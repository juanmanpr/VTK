#!/usr/bin/env python

## /*=========================================================================

##   Program:   Visualization Toolkit
##   Module:    HeaderTesting.py
##   Language:  Python
##   Date:      $Date$
##   Version:   $Revision$

##   Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
##   All rights reserved.
##   See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notice for more information.

## =========================================================================*/
## .NAME HeaderTesting - a VTK style and validity checking utility
## .SECTION Description
## HeaderTesting is a script which checks the list of header files for
## validity based on VTK coding standard. It checks for proper super
## classes, number and style of include files, type macro, private
## copy constructor and assignment operator, broken constructors, and
## exsistence of PrintSelf method. This script should be run as a part
## of the dashboard checking of the Visualization Toolkit and related
## projects.

## .SECTION See Also
## http://www.vtk.org http://public.kitware.com/Dart/HTML/Index.shtml
## http://www.vtk.org/contribute.php#coding-standards

import sys
import re
import os
import stat
import string

## For backward compatibility
def StringEndsWith(str1, str2):
    l1 = len(str1)
    l2 = len(str2)
    if l1 < l2:
        return 0
    return (str1[(l1-l2):] == str2)

##
class TestVTKFiles:
    def __init__(self):
        self.FileName = ""
        self.ErrorValue = 0;
        self.Errors = {}
        self.FileLines = []        
        pass
    def Print(self, text=""):
        rtext = text
        print rtext
    def Error(self, error):
        self.ErrorValue = 1
        self.Errors[error] = 1
        pass
    def PrintErrors(self):
        if self.ErrorValue:
            self.Print( )
            self.Print( "There were errors:" )
        for a in self.Errors.keys():
            self.Print( "* %s" % a )
        
    def TestFile(self, filename):
        self.FileName = filename
        self.FileLines = []
        self.ClassName = ""
        self.ParentName = ""
        try:
            file = open(filename)
            self.FileLines = file.readlines()
            file.close()
        except:
            self.Print( "Problem reading file: %s" % filename )
            sys.exit(1)
        pass
            
    def CheckIncludes(self):
        count = 0
        lines = []
        nplines = []
        includere = "^\s*#\s*include\s*[\"<]([^>\"]+)"
        ignincludere = ".*\/\/.*"
        regx = re.compile(includere)
        regx1 = re.compile(ignincludere)
        cc = 0
        includeparent = 0
        for a in self.FileLines:
            line = string.strip(a)
            rm = regx.match(line)
            if rm and not regx1.match(line):
                lines.append(" %4d: %s" % (cc, line))
                file = rm.group(1)
                if file == (self.ParentName + ".h"):
                    includeparent = 1
                if not StringEndsWith(file, ".h"):
                    nplines.append(" %4d: %s" % (cc, line))
            cc = cc + 1
        if len(lines) > 1:
            self.Print()
            self.Print( "File: %s has %d includes: " %
                        ( self.FileName, len(lines)) )
            for a in lines:
                self.Print( a )
            self.Error("Multiple includes")
        if len(nplines) > 0:
            self.Print( )
            self.Print( "File: %s has non-portable include(s): " % self.FileName )
            for a in nplines:
                self.Print( a )
            self.Error("Non-portabile includes")
        if not includeparent and self.ParentName:
            self.Print()
            self.Print( "File: %s does not include parent \"%s.h\"" %
                        ( self.FileName, self.ParentName ) )
            self.Error("Does not include parent")
        pass
    
    def CheckParent(self):
        classre = "^class\s*VTK_.*EXPORT (vtk[A-Z0-9][^ :\n]*)\s*:\s*public\s*(vtk[^ \n\{]*)"
        cname = ""
        pname = ""
        classlines = []
        regx = re.compile(classre)
        cc = 0
        for a in self.FileLines:
            line = string.strip(a)
            rm = regx.match(line)
            if rm:
                cname = rm.group(1)
                pname = rm.group(2)
                classlines.append(" %4d: %s" % (cc, line))
            cc = cc + 1
        if len(classlines) > 1:
            self.Print()
            self.Print( "File: %s defines %d classes: " %
                        (self.FileName, len(classlines)) )
            for a in lines:
                self.Print( a )
            self.Error("Multiple classes defined")
        if len(classlines) < 1:
            self.Print()
            self.Print( "File: %s does not define any classes" % self.FileName )
            self.Error("No class defined")
            return
        #self.Print( "Classname: %s ParentName: %s" % (cname, pname)
        self.ClassName = cname
        self.ParentName = pname
        pass
    def CheckTypeMacro(self):
        count = 0
        lines = []
        oldlines = []
        typere = "^\s*vtkType(Revision)*Macro\s*\(\s*(vtk[^ ,]+)\s*,\s*(vtk[^ \)]+)\s*\)\s*;"
        typesplitre = "^\s*vtkType(Revision)*Macro\s*\("
        
        regx = re.compile(typere)
        regxs = re.compile(typesplitre)
        cc = 0
        found = 0
        for a in range(len(self.FileLines)):
            line = string.strip(self.FileLines[a])
            rm = regx.match(line)
            if rm:
                found = 1
                if rm.group(1) != "Revision":
                    oldlines.append(" %4d: %s" % (cc, line))
                cname = rm.group(2)
                pname = rm.group(3)
                if cname != self.ClassName or pname != self.ParentName:
                    lines.append(" %4d: %s" % (cc, line))
            else:
                # Maybe it is in two lines
                rm = regxs.match(line)
                if rm:
                    nline = line + " " + string.strip(self.FileLines[a+1])
                    line = string.strip(nline)
                    rm = regx.match(line)
                    if rm:
                        found = 1
                        if rm.group(1) != "Revision":
                            oldlines.append(" %4d: %s" % (cc, line))
                        cname = rm.group(2)
                        pname = rm.group(3)
                        if cname != self.ClassName or pname != self.ParentName:
                            lines.append(" %4d: %s" % (cc, line))
            cc = cc + 1
        if len(lines) > 0:
            self.Print( "File: %s has broken type macro(s):" % self.FileName )
            for a in lines:
                self.Print( a )
            self.Print( "Should be:\n vtkTypeRevisionMacro(%s, %s)" %
                        (self.ClassName, self.ParentName) )
            self.Error("Broken type macro")
        if len(oldlines) > 0:
            self.Print( "File: %s has old type macro(s):" % self.FileName )
            for a in oldlines:
                self.Print( a )
                self.Print( "Should be:\n vtkTypeRevisionMacro(%s, %s);" %
                            (self.ClassName, self.ParentName))
            self.Error("Old style type macro")
        if not found:
            self.Print( "File: %s does not have type macro" % self.FileName )
            self.Print( "Should be:\n vtkTypeRevisionMacro(%s, %s);" %
                            (self.ClassName, self.ParentName))
            self.Error("No type macro")
        pass
    def CheckForCopyAndAssignment(self):
        if not self.ClassName:
            return
        count = 0
        lines = []
        oldlines = []
        copyoperator = "^\s*%s\s*\(\s*const\s*%s\s*&\s*\)\s*;\s*\/\/\s*Not\s*[iI]mplemented(\.)*" % ( self.ClassName, self.ClassName)
        asgnoperator = "^\s*void\s*operator=\s*\(\s*const\s*%s\s*&\s*\)\s*;\s*\/\/\s*Not\s*[iI]mplemented(\.)*" % self.ClassName
        #self.Print( copyoperator
        regx1 = re.compile(copyoperator)
        regx2 = re.compile(asgnoperator)
        foundcopy = 0
        foundasgn = 0
        for a in self.FileLines:
            line = string.strip(a)
            if regx1.match(line):
                foundcopy = foundcopy + 1
            if regx2.match(line):
                foundasgn = foundasgn + 1
            
        if foundcopy < 1:
            self.Print( "File: %s does not define copy constructor" %
                        self.FileName )
            self.Print( "Should be:\n%s(const %s&); // Not implemented" %
                        (self.ClassName, self.ClassName) )
            self.Error("No private copy constructor")
        if foundcopy > 1:
            self.Print( "File: %s defines multiple copy constructors" %
                        self.FileName )
            self.Error("Multiple copy constructor")
        if foundasgn < 1:
            self.Print( "File: %s does not define assignment operator" %
                        self.FileName )
            self.Print( "Should be:\nvoid operator=(const %s&); // Not implemented"
                        % self.ClassName )
            self.Error("No private assignment operator")
        if foundcopy > 1:
            self.Print( "File: %s defines multiple assignment operators" %
                        self.FileName )
            self.Error("Multiple assignment operators")
        pass
    def CheckWeirdConstructors(self):
        count = 0
        lines = []
        oldlines = []
        constructor = "^\s*%s\s*\(([^ )]*)\)" % self.ClassName
        copyoperator = "^\s*%s\s*\(\s*const\s*%s\s*&\s*\)\s*;\s*\/\/\s*Not\s*implemented(\.)*" % ( self.ClassName, self.ClassName)
        regx1 = re.compile(constructor)
        regx2 = re.compile(copyoperator)
        cc = 0
        for a in self.FileLines:
            line = string.strip(a)
            rm = regx1.match(line)
            if rm:
                arg = string.strip(rm.group(1))
                if arg and not regx2.match(line):
                    lines.append(" %4d: %s" % (cc, line))
            cc = cc + 1
        if len(lines) > 0:
            self.Print( "File: %s has weird constructor(s):" % self.FileName )
            for a in lines:
                self.Print( a )
            self.Print( "There should be only:\n %s();" % self.ClassName )
            self.Error("Wierd constructor")
        pass
    
    def CheckPrintSelf(self):
        if not self.ClassName:
            return
        typere = "^\s*void\s*PrintSelf\s*\(\s*ostream\s*&\s*os*\s*,\s*vtkIndent\s*indent\s*\)"
        newtypere = "^\s*virtual\s*void\s*PrintSelf\s*\(\s*ostream\s*&\s*os*\s*,\s*vtkIndent\s*indent\s*\)"
        regx1 = re.compile(typere)
        regx2 = re.compile(newtypere)
        found = 0
        oldstyle = 0
        for a in self.FileLines:
            line = string.strip(a)
            rm1 = regx1.match(line)
            rm2 = regx2.match(line)
            if rm1 or rm2:
                found = 1
                if rm1:
                    oldstyle = 1
        if not found:
            self.Print( "File: %s does not define PrintSelf method:" %
                        self.FileName )
            self.Error("No PrintSelf method")
        pass
        

## 
test = TestVTKFiles()

## Check command line arguments
if len(sys.argv) < 2:
    print "Testing directory not specified..."
    print "Usage: %s <directory> [ exception(s) ]" % sys.argv[0]
    sys.exit(1)
    
dirname = sys.argv[1]
exceptions = sys.argv[2:]

## Traverse through the list of files
for a in os.listdir(dirname):
    ## Skip non-header files
    if not StringEndsWith(a, ".h"):
        continue
    ## Skip exceptions
    if a in exceptions:
        continue
    pathname = '%s/%s' % (dirname, a)    
    if pathname in exceptions:
        continue
    mode = os.stat(pathname)[stat.ST_MODE]
    ## Skip directories
    if stat.S_ISDIR(mode):
        continue
    elif stat.S_ISREG(mode):
        ## Do all the tests
        test.TestFile(pathname)
        test.CheckParent()
        test.CheckIncludes()
        test.CheckTypeMacro()
        test.CheckForCopyAndAssignment()
        test.CheckWeirdConstructors()
        test.CheckPrintSelf()

## Summarize errors
test.PrintErrors()
sys.exit(test.ErrorValue)
    
