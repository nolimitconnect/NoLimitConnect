(function () {

  var githubLatestReleaseApi = 'https://api.github.com/repos/nolimitconnect/NoLimitConnect/releases/latest';
  var platforms = [
    {
      title: 'Windows',
      notes: 'NSIS installer for Windows x64.',
      icon: '/assets/icons/icon-windows.svg',
      artifactMatchers: [/\.exe$/i, /windows|x64|amd64|x86_64/i]
    },
    {
      title: 'Linux',
      notes: 'Debian package for Linux x64.',
      icon: '/assets/icons/icon-tux.svg',
      artifactMatchers: [/\.deb$/i, /x64|amd64|x86_64/i]
    },
    {
      title: 'Linux (ARM64)',
      notes: 'Debian package for Linux ARM64, including Raspberry Pi and Orange Pi devices.',
      icon: '/assets/icons/icon-orange-pi.svg',
      artifactMatchers: [/\.deb$/i, /arm64|aarch64/i]
    },
    {
      title: 'Android',
      notes: 'Signed APK intended for release distribution.',
      icon: '/assets/icons/icon-android.svg',
      artifactMatchers: [/\.apk$/i, /signed|release|android/i]
    },
    {
      title: 'Flatpak (x64)',
      notes: 'Flatpak bundle for Linux x64 desktops with Flatpak support.',
      icon: '/assets/icons/icon-ubuntu.svg',
      artifactMatchers: [/\.flatpak$/i, /x64|amd64|x86_64/i],
      allowGenericFlatpakName: true
    },
    {
      title: 'Flatpak (ARM64)',
      notes: 'Flatpak bundle for Linux ARM64 desktops with Flatpak support.',
      icon: '/assets/icons/icon-raspberry-pi.svg',
      artifactMatchers: [/\.flatpak$/i, /arm64|aarch64/i]
    }
  ];

  function formatUtc(isoUtc) {
    if (!isoUtc) {
      return 'Unknown';
    }

    var date = new Date(isoUtc);
    if (Number.isNaN(date.getTime())) {
      return 'Unknown';
    }

    return date.toISOString().replace('T', ' ').replace('.000Z', ' UTC');
  }

  function createPlatformSection(platform, payload, errorMessage) {
    var section = document.createElement('section');
    section.className = 'download-card';

    var headingWrap = document.createElement('div');
    headingWrap.className = 'download-card-title';

    if (platform.icon) {
      var icon = document.createElement('img');
      icon.src = platform.icon;
      icon.alt = platform.title + ' icon';
      icon.className = 'download-card-icon';
      icon.onerror = function () {
        this.style.display = 'none';
      };
      headingWrap.appendChild(icon);
    }

    var heading = document.createElement('h2');
    heading.textContent = platform.title;
    headingWrap.appendChild(heading);
    section.appendChild(headingWrap);

    var list = document.createElement('ul');

    if (payload && payload.artifact && payload.sha256) {
      var packageItem = document.createElement('li');
      packageItem.appendChild(document.createTextNode('Latest package: '));
      var packageLink = document.createElement('a');
      packageLink.href = payload.artifact.url;
      packageLink.textContent = payload.artifact.name;
      packageItem.appendChild(packageLink);
      list.appendChild(packageItem);

      var hashItem = document.createElement('li');
      hashItem.appendChild(document.createTextNode('SHA-256: '));
      var hashLink = document.createElement('a');
      hashLink.href = payload.sha256.url;
      hashLink.textContent = payload.sha256.name;
      hashItem.appendChild(hashLink);
      list.appendChild(hashItem);

      var updatedItem = document.createElement('li');
      updatedItem.textContent = 'Last updated: ' + formatUtc(payload.publishedAtUtc);
      list.appendChild(updatedItem);

      var notesItem = document.createElement('li');
      notesItem.textContent = 'Notes: ' + (payload.notes || platform.notes);
      list.appendChild(notesItem);
    } else {
      var missingItem = document.createElement('li');
      missingItem.textContent = 'No deployed package yet.';
      list.appendChild(missingItem);

      var platformNotesItem = document.createElement('li');
      platformNotesItem.textContent = 'Notes: ' + platform.notes;
      list.appendChild(platformNotesItem);

      if (errorMessage) {
        var errItem = document.createElement('li');
        errItem.textContent = 'Metadata status: ' + errorMessage;
        list.appendChild(errItem);
      }
    }

    section.appendChild(list);
    return section;
  }

  function findAssetByMatchers(assets, matchers, allowGenericFlatpakName) {
    for (var i = 0; i < assets.length; i += 1) {
      var asset = assets[i];
      var name = asset && asset.name ? asset.name : '';
      var matchesAll = true;

      for (var j = 0; j < matchers.length; j += 1) {
        if (!matchers[j].test(name)) {
          matchesAll = false;
          break;
        }
      }

      if (matchesAll) {
        return asset;
      }
    }

    if (allowGenericFlatpakName) {
      for (var k = 0; k < assets.length; k += 1) {
        if (/\.flatpak$/i.test(assets[k].name || '')) {
          return assets[k];
        }
      }
    }

    return null;
  }

  function buildPayloadFromRelease(platform, release) {
    var assets = Array.isArray(release.assets) ? release.assets : [];
    var artifactAsset = findAssetByMatchers(
      assets,
      platform.artifactMatchers || [],
      !!platform.allowGenericFlatpakName
    );

    if (!artifactAsset) {
      return null;
    }

    var expectedHashName = artifactAsset.name + '.sha256';
    var hashAsset = null;

    for (var i = 0; i < assets.length; i += 1) {
      if ((assets[i].name || '').toLowerCase() === expectedHashName.toLowerCase()) {
        hashAsset = assets[i];
        break;
      }
    }

    if (!hashAsset) {
      return null;
    }

    return {
      publishedAtUtc: release.published_at || release.created_at || null,
      artifact: {
        name: artifactAsset.name,
        url: artifactAsset.browser_download_url
      },
      sha256: {
        name: hashAsset.name,
        url: hashAsset.browser_download_url
      },
      notes: platform.notes
    };
  }

  function loadReleaseMetadata() {
    return fetch(githubLatestReleaseApi, { cache: 'no-store' })
      .then(function (response) {
        if (!response.ok) {
          throw new Error('HTTP ' + response.status);
        }
        return response.json();
      });
  }

  function loadPlatform(platform, release) {
    var payload = buildPayloadFromRelease(platform, release);

    if (payload) {
      return { platform: platform, payload: payload, error: null };
    }

    return { platform: platform, payload: null, error: 'No matching release asset found for this platform.' };
  }

  function renderDownloads() {
    var mount = document.getElementById('dynamic-downloads');
    if (!mount) {
      return;
    }

    mount.textContent = 'Loading latest package metadata from GitHub...';

    loadReleaseMetadata()
      .then(function (release) {
        var results = platforms.map(function (platform) {
          return loadPlatform(platform, release);
        });

        mount.innerHTML = '';
        mount.className = 'download-grid';

        results.forEach(function (result) {
          mount.appendChild(createPlatformSection(result.platform, result.payload, result.error));
        });
      })
      .catch(function (error) {
        mount.innerHTML = '';
        mount.className = 'download-grid';

        platforms.forEach(function (platform) {
          mount.appendChild(
            createPlatformSection(platform, null, 'GitHub release metadata unavailable: ' + error.message)
          );
        });
      });
  }

  if (typeof window.document$ !== 'undefined' && typeof window.document$.subscribe === 'function') {
    window.document$.subscribe(renderDownloads);
  } else {
    document.addEventListener('DOMContentLoaded', renderDownloads);
  }
})();
