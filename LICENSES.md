# ARES component licensing

ARES uses component-scoped licensing. This file clarifies scope; it does not relicense third-party dependencies or model weights.

- **Default project source:** the root `LICENSE` (GNU GPL-3.0).
- **Website source in `site/`:** additionally offered under `LICENSE-AGPL-3.0`. The AGPL grant applies only to the website source files in `site/`; it does not change the license of firmware, mobile app, AI core, Ada/SPARK verification sources, or unrelated project files.
- **Go web/API service in `bridge_service/`:** additionally offered under `LICENSE-AGPL-3.0`. This is limited to the service source in that directory. It does not automatically extend to other components that communicate with the service over standard protocols.
- **Gemma model weights:** are a separate third-party artifact governed by Google's applicable Gemma terms/license, not by either ARES source-code license. The quantized GGUF conversion also has its upstream repository's notices; review them before redistribution.

The AGPLv3 text is included verbatim in `LICENSE-AGPL-3.0`. License notices for the website and web service point to that file. This repository-level statement is a project intention, not legal advice; confirm copyright-holder authority before distributing under an additional license.
