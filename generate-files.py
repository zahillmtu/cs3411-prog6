# Author Scott Kuhl
#
# Intended for use a CS3411 assignment where students must write a
# program that finds duplicate files.

import os,shutil,random,string,getpass,time


root = "/tmp/cs3411-prog6-"+getpass.getuser()


# http://stackoverflow.com/questions/2257441/random-string-generation-with-upper-case-letters-and-digits-in-python
def randomString(N):
    """Generate a random string, used for filenames."""
    return ''.join(random.choice(string.ascii_lowercase + string.digits) for _ in range(N))


def getDirs():
    """Get a list of every directory."""
    dirs = [ root ]
    for dirName, subdirList, fileList in os.walk(root):
        dirs.append(dirName)
    return dirs

def getFiles():
    """Get a list of every file (including files in subdirectories)."""
    files = []
    for dirName, subdirList, fileList in os.walk(root):
        for f in fileList:
            files.append(os.path.join(dirName, f))
    return files
        

def makeFiles(numFiles, smallestFile, largestFile):
    """Make numFiles files randomly within the directory tree. Each file will contain random bytes ranging from size smallestFile to largestFile"""
    allDirs = getDirs()
    for i in range(numFiles):
        d = random.choice(allDirs)
        f = randomString(8)+".dat"
        filename = os.path.join(d, f)

        with open(filename, "w") as f:
            # print("File: %s" % filename)
            f.write(os.urandom(random.randint(smallestFile, largestFile)))


def makeDirs(numDirs):
    """Make numDir directories within the directory tree. Directories may be placed within other directories."""
    for i in range(numDirs):
        d = random.choice(getDirs())
        f = randomString(8)
        dirname = os.path.join(d, f)
        # print("Dir: %s" % dirname)
        os.mkdir(dirname)

def makeCopiedFiles(filesToBeCopied, numCopies):
    files = getFiles()
    dirs = getDirs()
    for i in range(filesToBeCopied):
        srcFile = random.choice(files) # select random file to copy
        for j in range(numCopies-1):
            randomDir  = random.choice(dirs)  # select random destination directory
            destFile = os.path.join(randomDir, randomString(8)+".dat")
            print("%s is a copy of %s" % (srcFile, destFile))
            shutil.copy(srcFile, destFile)

def fixTimestamps():
    # Without this, copied files would be newer than the other
    # files. Here, we sort the strings and then change each file in
    # order.
    for fname in sorted(getFiles()):
        if os.stat(fname).st_size > 0:  # keep 0 byte files 0 bytes
            byte = 0;
            with open(fname, "r") as f:
                byte = f.read(1)
            with open(fname, "r+") as f:
                f.write(byte)

        # Also force an update of the access time (not always updated
        # by the os on access).
        # http://stackoverflow.com/questions/19551139/access-time-does-not-change-after-a-file-is-opened
        now = int(time.time())
        os.utime(fname, (now,now))
             


if os.path.exists(root):
    print("Removing "+root)
    shutil.rmtree(root)
    
os.mkdir(root)


# These settings will likely change when the performance of submissions is measured.
makeDirs(100)                  # make directory tree
makeFiles(1000, 0, 1024)       # Generate files up to 1K
makeFiles(100,  0, 1024*1024)  # Generate files up to 1MB

# Our random number generator is slow, so generating large files is slower
# makeFiles(5,    0, 1024*1024*1024)   # generate files up to 1GB

makeCopiedFiles(3, 2)  # Make a duplicate of 3 different files
makeCopiedFiles(1, 3)  # make a triplicate of 1 file

fixTimestamps()

print("")
print("Files are in: "+root)
