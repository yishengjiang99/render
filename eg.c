#include <stdio.h>

#include "math.h"
#include "sf2/sf2.c"

typedef struct adsf {
  float att_rate_db, decay_rate, release_rate;
  float dbval, egval;
  int sr;
  int delay;
  int hold;
} adsr_t;

static inline float cent2samples(int ct, int sr) {
  if (ct <= -12000) return 49.0f;
  return (powf(2.0f, ct / 1200.0f) * sr);
}
void volEnv(adsr_t* env, zone_t* z, int sr) {
  env->att_rate_db = 960.0f / cent2samples(z->VolEnvAttack, sr);
  env->decay_rate =
      z->VolEnvSustain == 0 || z->VolEnvDecay <= -12000
          ? .0f
          : -1.0f * z->VolEnvSustain / cent2samples(z->VolEnvDecay, sr);

  env->hold = cent2samples(z->VolEnvHold, sr);
  env->delay = cent2samples(z->VolEnvDelay, sr);
  env->dbval = -960.0f;
  env->egval = 0.00001f;
  env->sr = sr;
}
void release(adsr_t* eg, int releaseCent) {
  eg->decay_rate = 0.0f;
  eg->hold = 0;
  eg->release_rate =
      (log(.000001) - log(1.0f)) / cent2samples(releaseCent, eg->sr);
}

int process(adsr_t* eg, double* ob, int n) {
  int att_ends_this_cycle = 0;
  if (eg->att_rate_db != 0 && eg->dbval + eg->att_rate_db * n >= 0.0f) {
    att_ends_this_cycle = 1;
  }
  for (int i = 0; i < n; i++) {
    if (eg->delay-- > 0) {
      ob[i] = eg->egval;
    } else if (att_ends_this_cycle) {
      eg->dbval += eg->att_rate_db;
      eg->egval = pow(10.0, eg->dbval * 0.005);
      if (eg->dbval >= 0.0f) {
        eg->att_rate_db = 0.0f;
        eg->dbval = 0.0f;
        eg->egval = 1.0f;
        att_ends_this_cycle = 0;
      }

    } else if (eg->att_rate_db > 0.000001f) {
      eg->dbval += eg->att_rate_db;
      eg->egval = pow(10.0, eg->dbval * 0.005);
    } else if (eg->decay_rate != 0.0f) {
      eg->egval += eg->decay_rate * eg->egval;
      if (eg->egval == 0.0f) eg->decay_rate = 0.0f;
    } else if (eg->release_rate != 0.f) {
      eg->egval += eg->release_rate * eg->egval;
    } else {
      // eg->egval -= 0.0000001f * eg->egval;
    }
    ob[i] = eg->egval;
  }
  return eg->egval <= 0.0f;
}

int main() {
  readsf("file.sf2");
  PresetZones* pz = findPresetByName("Clarinet");
  adsr_t env[1];
  zone_t* z = pz->zones;
  volEnv(env, z, 48000);

  env->decay_rate, z->VolEnvDecay, 96.0f / cent2samples(z->VolEnvDecay, 48000);
  double ob[129];

  for (int i = 0; i < 2; i++) {
    if (process(env, ob, 128)) break;
  }
  release(env, z->VolEnvRelease);
  for (int i = 0; i < 2; i++) {
    if (process(env, ob, 128)) break;
  }
}
