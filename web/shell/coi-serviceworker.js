// Cross-origin isolation shim.
//
// POV-Ray uses threads. In the browser, threads require SharedArrayBuffer,
// which is available only on "cross-origin isolated" pages, that is, pages
// served with
//
//   Cross-Origin-Opener-Policy: same-origin
//   Cross-Origin-Embedder-Policy: require-corp
//
// Static hosts such as GitHub Pages cannot set these headers. When the page is
// not isolated, this script registers itself as a service worker that adds the
// headers to every response from this origin, then reloads the page once.
// When the server already sends the headers (e.g. `web/tools/serve.py`), it
// does nothing.
//
// Adapted from coi-serviceworker by Guido Zuidhof
// (https://github.com/gzuidhof/coi-serviceworker), which is licensed
// under the following terms:
//
// Copyright (c) 2021 Guido Zuidhof
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/* eslint-env browser, serviceworker */
'use strict';

if (typeof window === 'undefined') {
  // ---- Service worker context ----
  self.addEventListener('install', () => self.skipWaiting());
  self.addEventListener('activate', (event) => event.waitUntil(self.clients.claim()));

  self.addEventListener('fetch', (event) => {
    const request = event.request;
    if (request.cache === 'only-if-cached' && request.mode !== 'same-origin') return;
    // Only touch same-origin requests; everything this app loads is same-origin.
    if (new URL(request.url).origin !== self.location.origin) return;

    event.respondWith(
      fetch(request).then((response) => {
        if (response.status === 0) return response;
        const headers = new Headers(response.headers);
        headers.set('Cross-Origin-Opener-Policy', 'same-origin');
        headers.set('Cross-Origin-Embedder-Policy', 'require-corp');
        headers.set('Cross-Origin-Resource-Policy', 'same-origin');
        return new Response(response.body, {
          status: response.status,
          statusText: response.statusText,
          headers,
        });
      }),
    );
  });
} else {
  // ---- Page context ----
  (() => {
    const reloadedKey = 'povray-web-coi-reloaded';
    if (window.crossOriginIsolated) {
      try { sessionStorage.removeItem(reloadedKey); } catch (e) { /* ignore */ }
      return;
    }
    if (!window.isSecureContext || !('serviceWorker' in navigator)) return;

    const script = document.currentScript && document.currentScript.src;
    if (!script) return;

    navigator.serviceWorker.register(script).then((registration) => {
      const reloadOnce = () => {
        let reloaded = false;
        try { reloaded = sessionStorage.getItem(reloadedKey) === '1'; } catch (e) { /* ignore */ }
        if (reloaded) return;  // avoid reload loops if isolation still fails
        try { sessionStorage.setItem(reloadedKey, '1'); } catch (e) { /* ignore */ }
        window.location.reload();
      };
      if (registration.active && !navigator.serviceWorker.controller) {
        reloadOnce();
      } else if (!registration.active) {
        const worker = registration.installing || registration.waiting;
        if (worker) {
          worker.addEventListener('statechange', () => {
            if (worker.state === 'activated') reloadOnce();
          });
        }
      } else if (navigator.serviceWorker.controller) {
        // Controlled but still not isolated (e.g. a stale worker): try once more.
        reloadOnce();
      }
    }).catch((err) => {
      console.warn('POV-Ray Web: could not register the cross-origin isolation service worker:', err);
    });
  })();
}
