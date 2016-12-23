===================================
===================================
 GCFScape BY: RYAN "NEMESIS" GREGG
===================================
===================================

===========================
Program/Author Information:
===========================

---- General Program Information ----
Date                     : October 17th, 2013
Author                   : Ryan "NEMESIS" Gregg
Title                    : GCFScape
Build                    : 1.8.5
Email address            : ryansgregg@hotmail.com
Home page /  Website     : http://nemesis.thewavelength.net/

---- Program Construction Information ----
Written In               : C++ .NET

=============
Requirements:
=============

  OS
  - Windows 7
  - Windows Vista
  - Windows XP

====
FAQ:
====

Q.
  When I launch GCFScape I get the following error message:
  "The application failed to initialize properly (0xc0000135). Click ok
  to terminate the application."

A.
  GCFScape is written in C++ .NET (using Visual C++ 2008 SP1) and as such
  requires the .NET 2.0 Framework and Visual C++ 2008 SP1 runtime to run.

  If you do not have the .Net Framework installed, are unsure what to do,
  or are having problems, please install the .Net 3.5 Framework which
  contains all the necessary runtimes:

  All architectures:
  http://www.microsoft.com/downloads/details.aspx?FamilyId=AB99342F-5D1A-413D-8319-81DA479AB0D7

  If you already have the .Net 2.0 Framework installed, then all you need
  is the updated C++ runtime.  See the following links for more information:
  
  x86 architectures (choose this if you are unsure):
  http://www.microsoft.com/downloads/details.aspx?familyid=A5C84275-3B97-4AB7-A40D-3802B2AF5FC2
  
  x64 architectures:
  http://www.microsoft.com/downloads/details.aspx?familyid=BA9257CA-337F-4B40-8C14-157CFDFFEE4E


Q.
  Are GCF files compressed archives?

A.
  No.


Q.
  What are GCF files then?

A.
  GCF (Assumed to stand for Game Cache File) files can best be compared to a
  virtual file system not unlike FAT or other common file systems. The virtual
  file system supports meta data, file fragments, checksums and other minor
  features. GCF files are the backbone of Steam's content delivery system.


Q.
  Why do I have to close Steam to open a GCF file?

A.
  GCFScape uses a technique called file mapping to directly access data in a
  GCF file as you would data in memory. This is a useful technique because it
  means GCFScape doesn't have to read tons of data into memory to read a GCF
  file and it can read a GCF file without having to seek all over the place.

  Because Steam opens GCF files in read/write mode and is liable to change the
  contents of any GCF file at any time, GCFScape requires that you close Steam.
  If Steam was open and it changed the contents of a GCF file while it was mapped,
  GCFScape would become unstable and likely crash.


Q.
  When I open a package I get the following error message:
  "Failed to open file."

A.
  GCFScape requires strict read rights to a package file to maintain stability.
  Make sure the package file is not open in any other program including Steam.
  As a work around you can enable Volatile Access from the Options menu.  Note:
  this may cause GCFScape to become unstable if Steam is updating the package you
  have open.


Q.
  When I open a GCF file I get the following error message:
  "Invalid GCF version number (vX)."

A.
  GCFScape does not know how to read the GCF file version you are attempting to
  open. This could be because it is an old GCF file version that I haven't yet
  added support for (due to the fact that I don't know what the format is) or a
  new GCF file version that a recent Steam update produced. Either way let me know.


Q.
  When I open a package I get the following error message:
  "Corrupt file."

A.
  GCFScape will display this error when it maps a package file to memory only to
  find there is not enough data in the file for the resulting map. This may or may
  not mean your file is corrupt but almost certainly means GCFScape would crash it
  continued to load your package file.


Q.
  When I open a package I get the following error message:
  "Failed to map view of file."

A.
  GCFScape will display this error when the system call MapViewOfFile() fails.
  This can happen on older operating systems with primitive virtual memory management.
  As a work around you can disable File Mapping from the Options menu.   Note: disabling
  File Mapping will incur a memory and performance penalty.


Q.
  When I press the Extract button, a Browse for Folder dialog appears but there is
  no directory tree to allow me to select a folder.

A.
  There is an unfortunate bug with the .NET framework that causes this dialog to
  operate incorrectly. This is a pretty rare occurrence, but there is a workaround:

   1. Close GCFScape.
   2. Open the gcfscape.cfg file, located in your GCFScape directory, in notepad.
   3. Set the GCFScape.ExtractDirectory argument to a folder on your hard drive such as C:\.
   4. Open GCFScape and extract your file.


Q.
  When I extract a file I get the following error message:
  "Error writing [filename] (File contains no physical data to write)."

A.
  This error occurs when you attempt to extract a file that has no physical data associated
  with it. This is a file that exists in the directory tree but which Steam has not yet
  downloaded. Use Steam to make sure the file is fully acquired.


Q.
  How do I create a GCF file?

A.
  As of yet, not enough is known about the GCF file format to programmatically create one.
  There are still plenty of unknown fields that need an understanding before a GCF file can
  be created. That said there is also no guarantee that creating a GCF file is all that is
  needed for Steam to recognize and make use of it. Some other registration might be required
  and may not be possible without hacking Steam. For these reasons I have no plans to add the
  ability to create a GCF file.


Q.
  How do I modify a GCF file?

A.
  The purpose of the GCF file is to make it easy for Steam to update Valve's software.
  When a new version of a program (say CS) is released, all Steam has to do is look in
  the CS GCF file and compare versions (hashes) of each file to determine if each file
  needs to be added, updated or removed. Because of this, if you were to modify a file,
  Steam would interpret this as meaning that the file is out of date and redownload
  the correct version of the file.

  Steam does, however, provide an interface to add or overwrite a file. To add or
  overwrite a file simply place it in the corresponding directory in the
  Steam\SteamApps\[EMail]\[GCF File] directory on your hard drive. You might have to
  create the appropriate subdirectories.


Q.
  Will GCFScape be able to create or modify GCF files in the future?

A.
  Not likely.

==================
Program Changelog:
==================

  v1.8.5
  - Added support for new SGA file format (v6).
  - Added support for new SGA file format (v7).

  v1.8.4
  - Fixed support for VPK file format (v1).

  v1.8.3
  - Added support for new VPK file format (v2).
  - Added SGA file support.
  - Added ZIP deflate support.
  - Fixed crash when opening VBSP files with no pak file lump.
  - Upgraded to HLLib v2.4.3.

  v1.8.2
  - Added support for GCF files larger than 4 GB.
  - Upgraded to HLLib v2.4.0.

  v1.8.1
  - Added Batch Validation tool.

  v1.8.0
  - Added support for x64.
  - Upgraded to HLLib v2.3.0.
  - Fixed crash on shutdown.

  v1.7.5
  - Added support for packages larger than 4 GB.
  - Added Open Containing Folder to context menu.
  - Upgraded to HLLib v2.2.0.
  - Fixed formatting of "Items" in the Properties form.

  v1.7.4
  - Upgraded to HLLib v2.1.2.

  v1.7.3
  - Upgraded to HLLib v2.1.1.

  v1.7.2
  - Fixed a crash sorting columns when opening a file of a different type than a previously opened file.

  v1.7.1
  - Upgraded to HLLib v2.1.0.
  - Added VPK file support.

  v1.7.0
  - Upgraded to HLLib v2.0.11.
  - Fixed support for files over 2 GB.

  v1.6.9
  - Added Fragmentation Report tool.
  - Added Batch Defragment tool.
  - Added regular expression matching.
  - Added Disable Warnings option.
  - Fixed wildcard matching.
  - Splitters now remember their locations.

  v1.6.8
  - Upgraded to HLLib v2.0.10.

  v1.6.7
  - Added program selection for unassociated file types.
  - Upgraded to HLLib v2.0.9.

  v1.6.6
  - Upgraded to HLLib v2.0.8.

  v1.6.5
  - Added NCF path auto setting and clearing.
  - Added file type icon to properties dialog.
  - Added close to File menu.
  - Added clear history to Recent Files menu.
  - Added validate to Tools menu.
  - Added error message for failed associations/disassociations.
  - Improved performance.

  v1.6.4
  - Added additional search features.
  - Added the ability to associate GCFScape with various formats.
  - Upgraded to HLLib v2.0.7.

  v1.6.3
  - Added configurable columns for custom properties.
  - Upgraded to HLLib v2.0.6.

  v1.6.2
  - Fixed bug in GCF file defragmentation progress.
  - Upgraded to HLLib v2.0.4.

  v1.6.1
  - Added NCF file support.
  - Upgraded to HLLib v2.0.3.

  v1.6.0
  - Added the ability to defragment GCF files.
  - Added check for missing recent files.
  - Fixed initial menu item state bug.
  - Fixed potential threading synchronization bug.
  - Upgraded to HLLib v2.0.2.

  v1.5.1
  - Added the ability to pause extraction and validation.
  - Improved drag & drop functionality.
  - Upgraded to HLLib v2.0.1.

  v1.5.0
  - Added the ability to validate packages.
  - Improved accessibility to package and item attributes.
  - Improved progress dialog.
  - Improved default setting selection.
  - Upgraded to HLLib v2.0.0.

  v1.4.1
  - Added XZP file support.
  - Upgraded to HLLib v1.1.8.

  v1.4.0
  - Upgraded to .NET v2.0.
  - Upgraded to HLLib v1.1.7.

  v1.3.1
  - Improved extraction progress form.
  - Fixed properties form refresh bug.

  v1.3.0
  - Added Completion attribute to properties form.
  - Upgraded to HLLib v1.1.6.

  v1.2.9
  - Added support for shell icons and descriptions.
  - Improved WAD file support.
  - Various optimizations.
  - Upgraded to HLLib v1.1.5.

  v1.2.8
  - Added support for wildcards in search strings.
  - Added support for Windows temporary folders.
  - Improved detection of corrupt packages.
  - Upgraded to HLLib v1.1.4.

  v1.2.7
  - Added multiple file selection.
  - Improved drag & drop functionality.

  v1.2.6
  - Added volatile access mode.
  - Files of size 0 B are now treated as valid files.
  - Upgraded to HLLib v1.1.3.
  - Updated Nem's Tools link.

  v1.2.5
  - Added Verbose console output mode.  (Slows down extracting.)
  - Added disable file mapping option.
  - Incomplete files are now grayed.
  - HLLib improvements.

  v1.2.4
  - Added tool bar.
  - Added address bar.
  - Added back, forward and up buttons.
  - Added support for back and forward mouse buttons.
  - Encrypted files are now grayed.
  - Other GUI improvements.

  v1.2.3
  - HLLib fixes.

  v1.2.2
  - Workaround for .NET FolderBrowserDialog bug.
  - Forced garbage collection.
  - HLLib fixes.

  v1.2.1
  - Added support for BSP packages.
  - Double clicking a file shell executes it.
  - Added ability to look up extensions in registry.
  - Fixed HLLib initialization bug.

  v1.2.0
  - Reprogrammed to use HLLib.
  - Added support for PAK and WAD packages.
  - Column headers can sort listbox items.

  v1.1.6
  - Altered file mapping algorithm to map small blocks instead of one large one.
  - Added checks for invalid Windows directory names.

  v1.1.5
  - Removed Beta status.
  - Added property form.
  - Removed 'Local Copy' column.

  v1.1.4
  - Added support for version 3 GCF files.
  - Added new panel in status bar which displays the GCF version number.

  v1.1.3.1
  - On error, useful GCF file data is dumpped to "dump.txt".

  v1.1.3
  - Figured out GCF versioning and added support for versions 5 and 6.
  - Improved error handling.

  v1.1.2
  - Modified the program to allow it to work with a Steam update (June 21, 2004).

  v1.1.1
  - Modified the program to allow it to work with a Steam update (June 16, 2004).
  - Added 'Local Copy' column.  If true a local copy of the file should exist on your hard drive.
  - Added 'Go To Directory' button. This will allow users to find the directory a search result is located in.

  v1.1.0
  - Added the ability to search for files within a GCF file.
  - Added extraction progress bar and abort button.
  - Added the ability to select and extract multiple items in the list box.
  - Extracting several items no longer visually locks up GCFScape.
  - GCFScape now remembers its position, window state and last extraction directory.
  - Fixed bug which cased crash when shell executing a file without a default application.

==============================
Program Copyright-Permissions:
==============================

LICENSE 

Terms and Conditions for Copying, Distributing, and Modifying 

Items other than copying, distributing, and modifying the Content
with which this license was distributed (such as using, etc.) are
outside the scope of this license. 

1. You may copy and distribute exact replicas of GCFScape as you receive
   it, in any medium, provided that you conspicuously and appropriately
   publish on each copy an appropriate copyright notice and disclaimer
   of warranty; keep intact all the notices that refer to this License
   and to the absence of any warranty; and give any other recipients of
   GCFScape a copy of this License along with GCFScape. You may at your
   option charge a fee for the media and/or handling involved in creating
   a unique copy of the GCFScape for use offline, you may at your option offer
   instructional support for the GCFScape in exchange for a fee, or you may at
   your option offer warranty in exchange for a fee. You may not charge a
   fee for GCFScape itself. You may not charge a fee for the sole service
   of providing access to and/or use of GCFScape via a network (e.g. the Internet),
   whether it be via the world wide web, FTP, or any other method. 

2. You may not modify your copy or copies of GCFScape or any portion of it.

3. You are not required to accept this License, since you have not signed it.
   However, nothing else grants you permission to copy, distribute or modify GCFScape.
   These actions are prohibited by law if you do not accept this License. Therefore,
   by copying or distributing GCFScape you indicate your acceptance of this License to do
   so, and all its terms and conditions for copying, distributing and modifying GCFScape. 

NO WARRANTY 

4. BECAUSE GCFSCAPE IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
   FOR GCFSCAPE, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED
   IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE GCFSCAPE "AS IS" WITHOUT
   WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
   ENTIRE RISK OF USE OF THE GCFSCAPE IS WITH YOU. SHOULD GCFSCAPE PROVE FAULTY, INACCURATE, OR
   OTHERWISE UNACCEPTABLE YOU ASSUME THE COST OF ALL NECESSARY REPAIR OR CORRECTION. 

5. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY
   COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MIRROR AND/OR REDISTRIBUTE GCFSCAPE AS
   PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL,
   INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE GCFSCAPE,
   EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
