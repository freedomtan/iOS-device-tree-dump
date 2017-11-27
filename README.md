# iOS-device-tree-dump
Dump non-encrypted iOS device tree extracted from im4p

iOS device tree files are not encrypted after iOS 10. So, with [img4tool](https://github.com/xerub/img4tool.git) from @xerub , 
```
> img4  -image ./all_flash/DeviceTree.d22ap.im4p  /tmp/d22ap-dt.bin
```
Then we can do something like
```
> ./dtdump /tmp/d22ap-dt.bin
```
or 
```
> ./dtdump -v /tmp/d22ap-dt.bin
```
`-v` means dump values in hexdecimal 
