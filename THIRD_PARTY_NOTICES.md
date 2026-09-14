# Upstream sources

LilyGO reference commit: `af0b94d6e79280bb6e8938ce3383ac19687957ca` in
[LilyGo-Cam-ESP32S3](https://github.com/Xinyuan-LilyGO/LilyGo-Cam-ESP32S3/tree/af0b94d6e79280bb6e8938ce3383ac19687957ca).

- `examples/MinimalCameraExample/MinimalCameraExample.ino`: AXP2101 camera rails, camera initialization and orientation.
- `examples/MinimalCameraExample/utilities.h`: exact `LILYGO_ESP32S3_CAM_PIR_VOICE` pin mapping.
- `examples/MinimalCameraExample/app_httpd.cpp`: Espressif multipart MJPEG streaming and separate HTTP servers.
- `README.md`: QIO 80 MHz, 16 MB flash, OPI PSRAM and Arduino 2.0.17 recommendation.

Local adaptations omit face detection, image conversion, controls and external assets; add a fixed AP, VGA-only configuration, send-rate limit, status page and error handling. Reference checkouts in `.reference/` are ignored and are not build dependencies.

## LilyGO portions (MIT)

Copyright (c) 2022 Shenzhen Xin Yuan Electronic Technology Co., Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Espressif portions (Apache-2.0)

Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Full text: `LICENSES/Apache-2.0.txt`.

XPowersLib is installed by PlatformIO with its own MIT license. Arduino-ESP32
and bundled ESP-IDF/camera components retain their upstream licenses.
