// also from Jonathan Levin's
// but originally from XNU source code I guess 

#define kPropNameLength	32

typedef struct DeviceTreeNodeProperty {
    char                name[kPropNameLength];  // NUL terminated property name
    uint32_t            length;         // Length (bytes) of folloing prop value
//  unsigned long       value[1];       // Variable length value of property
                                        // Padded to a multiple of a longword?
} DeviceTreeNodeProperty;

typedef struct OpaqueDTEntry {
    uint32_t            nProperties;    // Number of props[] elements (0 => end)
    uint32_t            nChildren;      // Number of children[] elements
//  DeviceTreeNodeProperty      props[];// array size == nProperties
//  DeviceTreeNode      children[];     // array size == nChildren
} DeviceTreeNode;

