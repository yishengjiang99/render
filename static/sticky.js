(function() {
'use strict';

let DEBUG = false;

/**
 * Returns true if the element's computed style is `position: sticky`.
 * @param {!Element} el
 * @return {boolean}
 */
function isSticky(el) {
  return getComputedStyle(el).position.match('sticky') !== null;
}

/**
 * Dispatches a `sticky-event` custom event on the element.
 * @param {boolean} stuck
 * @param {!Element} target Target element of event.
 */
function fire(stuck, target) {
  const evt = new CustomEvent('sticky-change', {detail: {stuck, target}});
  document.dispatchEvent(evt);
}

/**
 * @param {!Element} container
 */
function generatePage(container) {
  const MSGS = [
    'No',
    'JavaScript',
    'scroll events',
    'were',
    'used',
    'on',
    'this',
    'page!'
  ];

  const t = container.querySelector('template');
  const toc = document.querySelector('#toc > ul');
  const t2 = toc.querySelector('template');

  MSGS.forEach((msg, i) => {
    const clone = t.content.cloneNode(true);
    const h2 = clone.querySelector('h2');
    h2.textContent = msg;
    h2.id = normalizeTitle(msg);
    container.appendChild(clone);

    const tocClone = t2.content.cloneNode(true);
    const a = tocClone.querySelector('a');
    a.textContent = msg;
    a.href = `#${normalizeTitle(msg)}`;
    toc.appendChild(tocClone);
  });
}

/**
 * @param {!Element} container
 * @param {string} className
 */
function addSentinels(container, className) {
  return Array.from(container.querySelectorAll('.sticky')).map(el => {
    const sentinel = document.createElement('div');
    sentinel.classList.add('sticky_sentinel', className);
    return el.parentElement.appendChild(sentinel);
  });
}

/**
 * Sets up an intersection observer to notify when elements with the class
 * `.sticky_sentinel--top` become visible/invisible at the top of the container.
 * @param {!Element} container
 */
function observeHeaders(container) {
  const observer = new IntersectionObserver((records, observer) => {
    for (const record of records) {
      const targetInfo = record.boundingClientRect;
      const stickyTarget = record.target.parentElement.querySelector('.sticky');
      const rootBoundsInfo = record.rootBounds;

      if (targetInfo.bottom < rootBoundsInfo.top) {
        fire(true, stickyTarget);
      }

      if (targetInfo.bottom >= rootBoundsInfo.top &&
          targetInfo.bottom < rootBoundsInfo.bottom) {
       fire(false, stickyTarget);
      }
    }
  }, {
    // rootMargin: '-16px',
    threshold: [0],
    root: container
  });

  // Add the bottom sentinels to each section and attach an observer.
  const sentinels = addSentinels(container, 'sticky_sentinel--top');
  sentinels.forEach(el => observer.observe(el));
}

/**
 * Sets up an intersection observer to notify when elements with the class
 * `.sticky_sentinel--bottom` become visible/invisible at the botton of the
 * container.
 * @param {!Element} container
 */
function observeFooters(container) {
  const observer = new IntersectionObserver((records, observer) => {
    for (const record of records) {
      const targetInfo = record.boundingClientRect;
      const stickyTarget = record.target.parentElement.querySelector('.sticky');
      const rootBoundsInfo = record.rootBounds;
      const ratio = record.intersectionRatio;

      if (targetInfo.bottom > rootBoundsInfo.top && ratio === 1) {
        fire(true, stickyTarget);
      }

      if (targetInfo.top < rootBoundsInfo.top &&
          targetInfo.bottom < rootBoundsInfo.bottom) {
        fire(false, stickyTarget);
      }
    }
  }, {
    // rootMargin: '16px',
    // Get callback slightly before element is 100% visible/invisible.
    threshold: [1],
    root: container
  });

  // Add the bottom sentinels to each section and attach an observer.
  const sentinels = addSentinels(container, 'sticky_sentinel--bottom');
  sentinels.forEach(el => observer.observe(el));
}

/**
 * Notifies when elements that have the class `sticky` begin to stick or not.
 * Note: these should be children of the `container` element.
 */
function notifyWhenStickyHeadersChange(container) {
  observeHeaders(container);
  observeFooters(container);
}

function normalizeTitle(title) {
  return title.replace(/[\s!]/g, '-');
}

function scrollToHeader(el) {
  event.preventDefault();
  const target = document.querySelector(`#${normalizeTitle(el.textContent)}`);
  const parent = target.parentElement.parentElement;
  // Slight px offset so IntersectionObserver triggers.
  // Note: Smooth scroll is taken care of. Container has scroll-behavior: smooth.
  container.scrollTo({left: 0, top: parent.offsetTop + 2});
}

function toggleDebugMode() {
  DEBUG = !DEBUG;
  document.body.classList.toggle('debug', DEBUG);
  const sentinels = Array.from(document.querySelectorAll('.sticky_sentinel'));
  sentinels.forEach(el => {
    el.textContent = el.className;
  });
}

const container = document.querySelector('#container');
generatePage(container);

// Feature detect warning after page content is generated.
if (!isSticky(document.querySelector('.sticky')) || !window.IntersectionObserver) {
  document.querySelector('.nosupport').style.display = 'block';
} else  {
  notifyWhenStickyHeadersChange(container);
}

const whoIsSticky = document.querySelector('.who_is_sticky label');
const allTocsItems = Array.from(document.querySelectorAll('#toc .toc-item'));

document.addEventListener('sticky-change', e => {
  // Update sticking header title.
  const [header, stuck] = [e.detail.target, e.detail.stuck];
  header.classList.toggle('shadow', stuck);
  const str = stuck ? header.textContent : '--';
  whoIsSticky.textContent = str;

  // Update TOC selected item.
  allTocsItems.map(el =>  {
    const match = (el.firstElementChild.getAttribute('href').slice(1) ==
        header.firstElementChild.id);
    el.classList.toggle('active', match);
  });
});

if (location.search.includes('embed')) {
  document.body.classList.add('embed');
}

window.toggleDebugMode = toggleDebugMode;
window.scrollToHeader = scrollToHeader;
