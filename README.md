# tch-to-dds
```
It contains:

1) Shitty .tch -> .dds converter (TchToDdsConverter.cpp)
   It supports only DXT1, DXT3, DXT5, BC5, BC4, because it's shitty
   The program do not check file existence, because it's shitty
   If you want convert nor DXT1 nor DXT3 nor DXT5 nor BC5 nor BC4, you must use donor-dds
   Maybe, I will write later plugin to some kind of graphics editor
   Command line options:
     -tch <tch-file-as-input>
     -o <dds-file-as-input>
     -dds <dds-file-as-input> /*equal to -o <dds-file-as-input>*/
     -h /* shows help */
     -mipmapcount <number> /* set mip map count to number */
     -d <dds-file-as-donor> /* uses this dds file for dds header*/
```
