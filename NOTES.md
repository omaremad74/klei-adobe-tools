# Notes to know before reading on the formats

## FACINGS FORMAT
+ FACING_RIGHT      = 0000 0001 = 1 << 0
+ FACING_UP         = 0000 0010 = 1 << 1
+ FACING_LEFT       = 0000 0100 = 1 << 2
+ FACING_DOWN       = 0000 1000 = 1 << 3
+ FACING_UPRIGHT    = 0001 0000 = 1 << 4
+ FACING_UPLEFT     = 0010 0000 = 1 << 5
+ FACING_DOWNRIGHT  = 0100 0000 = 1 << 6
+ FACING_DOWNLEFT   = 1000 0000 = 1 << 7

## Hashing

Klei hashes their strings through this logic (For the games i've seen so far anyways, Invisible Inc seems to have different hashing logic it seems?):

```
def strhash(str):
    hash = 0
    for c in str:
        v = ord(c.lower())
        hash = (v + (hash << 6) + (hash << 16) - hash) & 0xFFFFFFFFL
    return hash
```

## Vertex

+ 'x' x-coordinate of the vertex
+ 'y' y-coordinate of the vertex
+ 'z' z-coordinate of the vertex. In the case of 2D animated games like DS/T or Rotwood this is always going to be 0!
+ 'u' 
+ 'v' 
+ 'w' texture/material sampler

## Naming schemes

'Root Symbols' and banks refer to one another.
'Materials' and atlases refer to one another.
'Folders' and layers refer to one another.

# Klei's Binary Formats

## Don't Starve / Together

### ANIM (Version 4)
+ "ANIM"
+ Version Number (uint32_t)
+ Total Num Element Refs (uint32_t)
+ Total Num Frames (uint32_t)
+ Total Num Events (uint32_t)
+ Num Animations (uint32_t)
+ For each animation:
    + Animation Name (uint32_t, string)
    + Valid Facings (byte bit mask) (see FACINGS FORMAT)
    + 'Root Symbol' Hash (uint32_t)
    + Frame Rate (float)
    + Num Frames (uint32_t)
    + For each frame:
        + x, y, width, height (all floats)
        + Num Events (uint32_t), This is deprecated. It's likely to be often 0.
            + For each event:
            + Event Hash
        + Num Elements (uint32_t)
            + Symbol Hash (uint32_t)
            + Symbol Frame (uint32_t)
            + Folder Hash (uint32_t)
            + Matrix: a, b, c, d, tx, ty, tz (All floats)
+ Num Hashed Strings (uint32_t)
+ For each hashed string:
    + Hash (uint32_t)
    + Original String (uint32_t, string)

### BILD (Version 6)
+ "BILD"
+ Version Number (uint32_t) 
+ Total Num Symbols (uint32_t)
+ Total Num Frames (uint32_t)
+ Build Name (uint32_t, string)
+ Num 'Materials'
+ For each material:
    + Texture Name (uint32_t, string)
+ For each symbol:
    + Symbol Hash (uint32_t)
    + Num Frames (uint32_t)
        + Frame Num (uint32_t)
        + Frame Duration (uint32_t)
        + Bounding Box: x, y, width, height (all floats)
        + Vertex Buffer Start Index (uint32_t)
        + Num Vertices (uint32_t)
+ Total Num Vertices (uint32_t)
+ For each vertex:
    + x, y, z, u, v, w (all floats)
+ Num Hashed Strings (uint32_t)
+ For each hashed string:
    + Hash (uint32_t)
    + Original String (uint32_t, string)

### KTEX 
+ "KTEX"
+ 32 bitset header for data (all interpreted as uint32_t)
    + Platform (4 bits)
    + Compression (5 bits)
    + Texture Type (4 bits)
    + Mipmap Count (5 bits)
    + Flags (2 bits)
    + Fill (12 bits)
+ For each MipMap:
    + Mipmap width (uint16_t)
    + Mipmap height (uint16_t)
    + Mipmap pitch (uint16_t)
    + Mipmap data size (uint32_t)
+ For each MipMap:
    Read mipmap data with data size

## Rotwood

### ANIM (Version 5) NOTE: This is labelled as version 5, but do not be fooled! It is not the same as Oxygen Not Includeds format despite also being labelled as version 5!
+ "ANIM"
+ Version Number (uint32_t)
+ Total Num Element Refs (uint32_t)
+ Total Num Frames (uint32_t)
+ Total Num Events (uint32_t)
+ Num Animations (uint32_t)
+ For each animation:
    + Animation Name (uint32_t, string)
    + Valid Facings (byte bit mask) (see FACINGS FORMAT)
    + 'Root Symbol' Hash (uint32_t)
    + Frame Rate (float)
    + Num Frames (uint32_t)
    + For each frame:
        + x, y, width, height (all floats)
        + Num Events (uint32_t)
            + For each event:
            + Event Hash
        + Num Elements (uint32_t)
            + Symbol Hash (uint32_t)
            + Symbol Frame (uint32_t)
            + Folder Hash (uint32_t)
            + ? (uint32_t) Sorry! But I don't know what these 4 values are! I believe this integer might be a flag/bit field though!
            + ? (float) These other floats I found to generally have a value of 1.0, i've yet to extrapolate what they might affect!
            + ? (float) 
            + ? (float)
            + Colour: a, b, g, r (all floats)
            + Matrix: a, b, c, d, tx, ty, tz (All floats)
+ Num Hashed Strings (uint32_t)
+ For each hashed string:
    + Hash (uint32_t)
    + Original String (uint32_t, string)

### BILD (Version 6)
+ "BILD"
+ Version Number (uint32_t) 
+ Total Num Symbols (uint32_t)
+ Total Num Frames (uint32_t)
+ Build Name (uint32_t, string)
+ Num 'Materials'
+ For each material:
    + Texture Name (uint32_t, string)
+ For each symbol:
    + Symbol Hash (uint32_t)
    + Num Frames (uint32_t)
        + Frame Num (uint32_t)
        + Frame Duration (uint32_t)
        + Bounding Box: x, y, width, height (all floats)
        + Vertex Buffer Start Index (uint32_t)
        + Num Vertices (uint32_t)
+ Total Num Vertices (uint32_t)
+ For each vertex:
    + x, y, z, u, v, w (all floats)
+ Num Hashed Strings (uint32_t)
+ For each hashed string:
    + Hash (uint32_t)
    + Original String (uint32_t, string)

### KTEX 
+ "KTEX"
+ some value, usually '2' (byte)
+ width (uint16_t)
+ height (uint16_t)
+ DDS file contents (variable size, read to the end of the file)

## Oxygen Not Included

### ANIM (Version 5)
+ "ANIM"
+ Version Number (uint32_t)
+ Total Num Element Refs (uint32_t)
+ Total Num Frames (uint32_t)
+ Num Animations (uint32_t)
+ For each animation:
    + Animation Name (uint32_t, string)
    + 'Root Symbol' Hash (uint32_t)
    + Frame Rate (float)
    + Num Frames (uint32_t)
    + For each frame:
        + x, y, width, height (all floats)
        + Num Elements (uint32_t)
            + Symbol Hash (uint32_t)
            + Symbol Frame (uint32_t)
            + Folder Hash (uint32_t)
            + Hide Head Animation Hash (uint32_t) NOTE: If this is 4100639152 (aka the hash of "head_anim") then some extra animation logic happens, Haha so weird!
                + Proof!: 
                    ```if (new KAnimHashedString(reader.ReadInt32()) == KGlobalAnimParser.ANIM_HASH_HEAD_ANIM)
                    {
                        frame.hasHead = true;
                    }
                    ```
            + Colour: a, b, g, r (all floats)
            + Matrix: a, b, c, d, tx, ty, tz (All floats)
            + Order (float) (I believe this may be deprecated?)
+ Max Visible Symbol Frames (uint32_t)
+ Num Hashed Strings (uint32_t)
+ For each hashed string:
    + Hash (uint32_t)
    + Original String (uint32_t, string)

### BILD (Version 10)
+ "BILD"
+ Version Number (uint32_t)
+ Total Symbols (uint32_t)
+ Total Frames (uint32_t)
+ Build Name (uint32_t, string)
+ For each symbol:
    + Symbol Hash (uint32_t)
    + Symbol Path Hash (uint32_t)
    + Colour Channel Hash (uint32_t)
    + Flags (uint32_t)
    + Num Frames (uint32_t)
    + For each frame:
        + Frame Num (uint32_t)
        + Frame Duration (uint32_t)
        + Build Image Index (uint32_t)
        + Bounding Box: x, y, width, height (all floats)
        + UV: minx, miny, maxx, maxy (all floats)
+ Num Hashed Strings (uint32_t)
+ For each hashed string:
    + Hash (uint32_t)
    + Original String (uint32_t, string)

### KTEX
+ No unique format! Just a png file! (In Unity's compressed resource files)

## Griftlands

### ANIM (Version 7)
+ "ANIM"
+ Version Number (uint32_t)
+ Total Num Element Refs (uint32_t)
+ Total Num Frames (uint32_t)
+ Num Animations (uint32_t)
+ For each animation:
    + Primary Animation Name (uint32_t, string)
    + Secondary Animation Name (uint32_t, string)
    + Frame Rate (float)
    + Flag (uint32_t)
    + Num Frames (uint32_t)
    + For each frame:
        + x, y, width, height (all floats)
        + Num Elements (uint32_t)
        + For each element:
            + Symbol Hash (uint32_t)
            + Symbol Frame (uint32_t)
            + Folder Hash (uint32_t)
            + Colour: a, b, g, r (all floats)
            + 16 Unknown Bytes's Sorry! I dunno them! (Likely 4 floats/ints but what they refer to I do not know.)
            + Matrix: a, b, c, d, tx, ty, tz (All floats)

NOTE: It seems Griftlands animations can have two names for animations.

### BILD (Version 10) NOTE: Again, not the same as Oxygen Not Included despite both being version '10', haha!
+ "BILD"
+ Version Number (uint32_t)
+ Total Symbols (uint32_t)
+ Total Frames (uint32_t)
+ Build Name (uint32_t, string)
+ Num 'Materials'
+ For each material:
    + Texture Name (uint32_t, string)
+ Flag (uint32_t)
+ For each symbol:
    + Symbol Hash (uint32_t)
    + Unknown 4 bytes, Potentially and likely another hash? (uint32_t)
    + Flag (uint32_t)
    + Num Frames (uint32_t)
    + For each frame:
        + Frame Num (uint32_t)
        + Frame Duration (uint32_t)
        + Build Image Index (uint32_t)
        + Bounding Box: x, y, width, height (all floats)
        + UV: minx, miny, maxx, maxy (all floats)
+ Num Hashed Strings (uint32_t)
+ For each hashed string:
    + Hash (uint32_t)
    + Original String (uint32_t, string)

### KTEX 
+ "KTEX"
+ some value, usually '2' (byte)
+ width (uint16_t)
+ height (uint16_t)
+ DDS file contents (variable size, read to the end of the file)

### PLAX
NOTES: 'What is plax?' It's Klei's implementation of parallax backgrounds! Despite being `.tex` they don't really have any texture data on their own 
they contain data referencing other actual `KTEX` textures to perform logic on with different configurations in usage for scene backgrounds as far as I understand.
I'm heavily referencing this lovely repositories work, I do not fully understand the format myself yet: https://github.com/instant-noodles/gltools/blob/master/src/gltex.py
+ "PLAX"
+ Num Root (byte)
+ Width (int32_t)
+ Height (int32_t)
+ Root Name (length in byte, string)
+ Num Splices (int32_t)
+ For each splice:
    + PBox: U1, V1, U2, V2, n_U1, n_V1, n_U2, n_V2 (all floats) (what's a pbox!?)

## Invisible Inc.

NOTES: Invisible Inc uses .KWAD files ("Klei, Where's All the Data?") that contain compressed animations, builds, and textures all together
It is unlikely I will get to supporting KWAD in a timely manner. Sorry! But the documentation for its format is here :3
For more information, check out this git!: https://github.com/Psimage/KWADTool/

Invisible Inc has two versions for its KWAD format, indicated by signatures at the first eight bytes, "KLEIPKG1" and "KLEIPKG2"

'Instances' refer to elements, this is how Invisible Inc documentation refers to it.

I'm unsure what 'Parent' refers to.

CM index and CA index seems to refer to specific color logic. I am lost here:
https://github.com/Psimage/KWADTool/pull/1/commits/9550bb7b843360ad876bf04a59e56ea7da05767f

For encoded_strings, see: `char[len + (4 - len % 4) % 4]  : string contents padded to maintain DWORD alignment`

### KWAD

### ANIM 
+ "ANM1"
+ Struct Size (uint32_t)
+ Num Animations (uint32_t)
+ For each animation:
    + Animation Name Hash (uint32_t)
    + Animation Name (char[20])
    + 'Root Symbol' Hash (uint32_t)
    + Frame Rate (float)
    + Valid Facings (byte bit mask) (see FACINGS FORMAT)
    + Frame Index (uint32_t)
    + Num Frames (uint32_t)
+ Total Num Frames (uint32_t)
+ For each frame:
    + Event Index (uint32_t)
    + Num Events (uint32_t)
    + 'Instance' Index (uint32_t)
    + Num 'Instances' (uint32_t)
+ Total Num Events (uint32_t)
+ For each event:
    + Event Hash (uint32_t)
+ Total Num 'Instances'
+ For each 'instance':
    + Symbol Hash (uint32_t)
    + Folder Hash (uint32_t)
    + Parent Hash (uint32_t)
    + Symbol Frame (uint32_t)
    + Parent Transform Index (uint32_t)
    + Transform Index (uint32_t)
    + CM Index (uint32_t)
    + CA Index (uint32_t)
+ Total Num Colours (uint32_t)
+ For each colour:
    + r, g, b, a (all floats)
+ Total Num Transform (uint32_t)
+ For each transform:
    + 3D Affine Matrix: (16 floats)
        (a, c, _, tx)
        (b, d, _, ty)
        (_, _, _, _ )
+ Event Strings (uint32_t, string)

### BILD
+ "BLD1"
+ Struct Size (uint32_t)
+ Build Name (uint32_t, encoded_string)
+ Total Num Symbols (uint32_t)
+ For each symbol:
    + Symbol Hash (uint32_t)
    + Symbol Name (char[20])
    + Frame Index (uint32_t)
    + Frame Count (uint32_t)
+ Total Num Frames (uint32_t)
+ For each frame:
    + Model Resource Index (uint32_t)
    + 3D Affine Matrix: (16 floats)
        (a, c, _, tx)
        (b, d, _, ty)
        (_, _, _, _ )

## Mark Of The Ninja
+ TODO

## Eets
+ TODO

## Shank
+ TODO
+ Shank uses xml and dds instead of binary format and ktex