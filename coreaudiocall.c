#if MACOSX_COREAUDIO
typedef void (*addDevFn)(const char *name, AudioDeviceID devId, void *data);

static void
addToDevList(const char *name, AudioDeviceID devId, void *data)
{
	SDL_AddAudioDevice addfn = (SDL_AddAudioDevice)data;
	addfn(name);
}

typedef struct
{
	const char *findname;
	AudioDeviceID devId;
	int found;
} FindDevIdData;

static void
findDevId(const char *name, AudioDeviceID devId, void *_data)
{
	FindDevIdData *data = (FindDevIdData *)_data;
	if (!data->found)
	{
		if (SDL_strcmp(name, data->findname) == 0)
		{
			data->found = 1;
			data->devId = devId;
		}
	}
}

static void
build_device_list(int iscapture, addDevFn addfn, void *addfndata)
{
	OSStatus result = noErr;
	UInt32 size = 0;
	AudioDeviceID *devs = NULL;
	UInt32 i = 0;
	UInt32 max = 0;

	AudioObjectPropertyAddress addr = {
			kAudioHardwarePropertyDevices,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMaster};

	result = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr,
																					0, NULL, &size);
	if (result != kAudioHardwareNoError)
		return;

	devs = (AudioDeviceID *)alloca(size);
	if (devs == NULL)
		return;

	result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr,
																			0, NULL, &size, devs);
	if (result != kAudioHardwareNoError)
		return;

	max = size / sizeof(AudioDeviceID);
	for (i = 0; i < max; i++)
	{
		CFStringRef cfstr = NULL;
		char *ptr = NULL;
		AudioDeviceID dev = devs[i];
		AudioBufferList *buflist = NULL;
		int usable = 0;
		CFIndex len = 0;

		addr.mScope = iscapture ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
		addr.mSelector = kAudioDevicePropertyStreamConfiguration;

		result = AudioObjectGetPropertyDataSize(dev, &addr, 0, NULL, &size);
		if (result != noErr)
			continue;

		buflist = (AudioBufferList *)SDL_malloc(size);
		if (buflist == NULL)
			continue;

		result = AudioObjectGetPropertyData(dev, &addr, 0, NULL,
																				&size, buflist);

		if (result == noErr)
		{
			UInt32 j;
			for (j = 0; j < buflist->mNumberBuffers; j++)
			{
				if (buflist->mBuffers[j].mNumberChannels > 0)
				{
					usable = 1;
					break;
				}
			}
		}

		SDL_free(buflist);

		if (!usable)
			continue;

		addr.mSelector = kAudioObjectPropertyName;
		size = sizeof(CFStringRef);
		result = AudioObjectGetPropertyData(dev, &addr, 0, NULL, &size, &cfstr);
		if (result != kAudioHardwareNoError)
			continue;

		len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfstr),
																						kCFStringEncodingUTF8);

		ptr = (char *)SDL_malloc(len + 1);
		usable = ((ptr != NULL) &&
							(CFStringGetCString(cfstr, ptr, len + 1, kCFStringEncodingUTF8)));

		CFRelease(cfstr);

		if (usable)
		{
			len = strlen(ptr);

			while ((len > 0) && (ptr[len - 1] == ' '))
			{
				len--;
			}
			usable = (len > 0);
		}

		if (usable)
		{
			ptr[len] = '\0';

#if DEBUG_COREAUDIO
			printf("COREAUDIO: Found %s device #%d: '%s' (devid %d)\n",
						 ((iscapture) ? "capture" : "output"),
						 (int)*devCount, ptr, (int)dev);
#endif
			addfn(ptr, dev, addfndata);
		}
		SDL_free(ptr);
	}
}

static void
COREAUDIO_DetectDevices(int iscapture, SDL_AddAudioDevice addfn)
{
	build_device_list(iscapture, addToDevList, addfn);
}

static int
find_device_by_name(_THIS, const char *devname, int iscapture)
{
	AudioDeviceID devid = 0;
	OSStatus result = noErr;
	UInt32 size = 0;
	UInt32 alive = 0;
	pid_t pid = 0;

	AudioObjectPropertyAddress addr = {
			0,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMaster};

	if (devname == NULL)
	{
		size = sizeof(AudioDeviceID);
		addr.mSelector =
				((iscapture) ? kAudioHardwarePropertyDefaultInputDevice : kAudioHardwarePropertyDefaultOutputDevice);
		result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr,
																				0, NULL, &size, &devid);
		CHECK_RESULT("AudioHardwareGetProperty (default device)");
	}
	else
	{
		FindDevIdData data;
		SDL_zero(data);
		data.findname = devname;
		build_device_list(iscapture, findDevId, &data);
		if (!data.found)
		{
			SDL_SetError("CoreAudio: No such audio device.");
			return 0;
		}
		devid = data.devId;
	}

	addr.mSelector = kAudioDevicePropertyDeviceIsAlive;
	addr.mScope = iscapture ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;

	size = sizeof(alive);
	result = AudioObjectGetPropertyData(devid, &addr, 0, NULL, &size, &alive);
	CHECK_RESULT("AudioDeviceGetProperty (kAudioDevicePropertyDeviceIsAlive)");

	if (!alive)
	{
		SDL_SetError("CoreAudio: requested device exists, but isn't alive.");
		return 0;
	}

	addr.mSelector = kAudioDevicePropertyHogMode;
	size = sizeof(pid);
	result = AudioObjectGetPropertyData(devid, &addr, 0, NULL, &size, &pid);

	if ((result == noErr) && (pid != -1))
	{
		SDL_SetError("CoreAudio: requested device is being hogged.");
		return 0;
	}

	SDLAUDIOHIDDEN->deviceID = devid;
	return 1;
}
#endif
