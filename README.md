# Open Here
SG Toolbox' Open Here Utility

This tool allows to open a program "where" the user is.

It detects running instances of the Windows File Explorer.
From the top-most instance it, fetches the opened path, and any selected files.
It then calls the configured program, with command line arguments and working directories set based on the fetched information.

If no instance of the Windows File Explorer is found, or no valid file system path could be extracted, then default values can be used when calling the configured program.

If no program is configured via command line arguments, a tool window is displayed to select one of up to twelve configured tools to be opened.


## Using the Tool Box Window

TODO


## Configuring the Tool Box Window

TODO


## Using the Command Line Options

TODO


## Building the Application

TODO


## Infos and Contact
The projects main repository is hosted on Github:

https://github.com/sgrottel/open-here

You can find more info about the author on my website:

https://www.sgrottel.de


## License
This project is freely available as open source under the Apache License V2 (see: [LICENSE](./LICENSE))

> Copyright 2022 SGrottel
>
> Licensed under the Apache License, Version 2.0 (the "License");
> you may not use this file except in compliance with the License.
> You may obtain a copy of the License at
>
> http://www.apache.org/licenses/LICENSE-2.0
>
> Unless required by applicable law or agreed to in writing, software
> distributed under the License is distributed on an "AS IS" BASIS,
> WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
> See the License for the specific language governing permissions and
> limitations under the License.
