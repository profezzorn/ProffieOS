
# create_POV_data_files (script)

Use ProffieOS POV Tools to create Persistence-Of-Vision images that show when you swing the blade.  
This script utilizes the existing Makefile in the pov_tools folder.  
Images can be created without this script via command line by typing 'make'.  
Arguments may be provided as 'OPTIONS=--height=N  --length=N --offset=N'  

Under the hood:   
pnmtorle - Converts a full-color image to hex data to be included in the code.  
pgmtorle - Converts a single-color image to hex data to be included in the code.  
pnmwindshieldwiper - Scales the image and bends it so that it will work on a swinging blade.  
pnmquantizedtorle - Quantizes images to 8bit 256 colors. This strikes a balance of 
                    using color images and not taking up too much memory.  

Operating System Requirements:

### Windows (Windows 10 at time of writing):   
Linux subsystem for Windows is required before usage.
- Open Windows Powershell as administrator and then simply type in the following: wsl --install
- Let Powershell run and wait until it's finished and suggests you reboot/restart your computer.  
- Restart your computer. Ubuntu will install itself after you log back in.

Install g++ and netpbm
- Type “WSL” into the windows search and click the WSL app with the penguin icon. 
- In the window that appears type: sudo apt-get update
- When that finishes, type: sudo apt-get install build-essential gdb  
- Answer the question with "y".  
- When that finishes, type: sudo apt install netpbm 
- Answer the question with "y".  

### MacOS:
- Install Homebrew https://treehouse.github.io/installation-guides/mac/homebrew  
- Install netpbm (In Terminal, type "brew install netpbm").  

### Linux
- Install netpbm http://netpbm.sourceforge.net/  

---------------------------------------------------------------------------------------

### Make POV files

- Put one or more .png images that you want to convert into the pov_tools folder.  
  If you have no images, you can run the script anyway and it can download the 
  sample Star Wars logo and process it for you :)  
  File names that contain 'preview' will be ignored as valid source image files. 

To run the script:  
- Windows: Double click "create_POV_data_files-Win.bat"  
    (If Microsoft Defender pops up a warning, click "More Info" then "Run anyway")  
- Mac or Linux: Double click "create_POV_data_files"  

- Choose a sizing option from the list.
  There are some preset sizes plus 'Height entry only' and 'Custom size' options.  
  (Height is the number of pixels in the blade)  
  You may want to choose 'Custom' first to get an idea of what gets calculated,
  then the presets will make more sense.
  Once familiar, you can customize the presets by editing that section of the script.  

All valid .png files in the pov_tools folder will be processed to the setting you chose.
- Check the resulting *preview.png in the pov_tools folder.
  It should look warped like a windshield shape.  
- The image data files are written directly to the ProffieOS/styles folder.  
  They will be formatted "sourceFileName_height_colorProfile_POV_data.h"  
  If you used a Single Color source file, use the resulting SC_POV_data.h file.  
  If you used a Full Color source file, then use the resulting FC_POV_data.h file.  
  If you used a Full Color source file, memory use might be less by choosing the 8b_POV_data.h file.  
- A running log of your conversion history with the setting used is written.
  See the file at ProffieOS/pov_tools/POV_conversions.log.  

- To use the POV data file for a blade style,
  first tell the OS which one with the following line added to your config file  
  in the CONFIG_TOP section: `#define POV_INCLUDE_FILE "dataFileNameHere.h"`  

- Then, to use the POV blade style in a preset, you have a few choices:
  If you want one instance of the image to display in the top portion of the swing "window", 
  the templates to use is StylePOV<>.
  StylePOV<> can take a REPEAT argument that will create additional instances of
  the image. See the Usage section of the styles/pov.h file.
  If you want to show the image multiple times continuously through 360 degrees of the swing,
  use ContinuousPOV<>.

1. Use it as a stand-alone style in a preset. Example syntax is shown.   

{ "Font", "tracks/track.wav",  
  StylePtr<StylePOV<>>(),  
  "my_pov"}  
  
2. Use it inside a blade style. It could be the base blade "color", 
   or something that shows when swinging,  
   or when triggering an effect of your choice, etc... 
   Basic SwingSpeed<> example is shown below.  

{ "Font", "tracks/track.wav",  
  StylePtr<Layers<
  Red,
  AlphaL<ContinuousPOV<>,SwingSpeed<400>>,
  ResponsiveLockupL<White,TrInstant,TrInstant,Int<26000>>,
  InOutTrL<TrWipe<300>,TrWipeIn<500>>>>(),
  "my_continuous_pov"}  

Swing the blade in a steady, quick movement to see the image in the air. 
A long-exposure camera capture showcases the result best.
