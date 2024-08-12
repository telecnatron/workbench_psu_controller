# ------------------------------------------------------------------------------
# Copyright 2020 Stephen Stebbing. telecnatron.com
#
#    Licensed under the Telecnatron License, Version 1.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        https://telecnatron.com/software/licenses/
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
# ------------------------------------------------------------------------------
class Memory():
    """ """

    def __init__(self, sizeBytes, pageSizeBytes=0):
        ""
        # array of bytes representing the memory
        self.memory=bytearray();
        self.size=sizeBytes;
        self.pageSize = pageSizeBytes;
        # initialise all bytes to 0xff
        for a in range(sizeBytes):
            self.memory.append(0xff);


    def getPage(self, pageNum):
        """Return bytearray being the data contained in the page with the passed pageNum"""
        pageAddr= int(pageNum * self.pageSize);
        pageAddrEnd = pageAddr + self.pageSize;

        # XXX this don't work for pagenum > 0, I'm missing something
        #  page = self.memory[pageNum:pageAddrEnd];
        # XXX we do it ourselves
        page=bytearray(self.pageSize);
        i=0;
        for b in range(pageAddr, pageAddrEnd):
            page[i]=self.memory[b];
            i+=1;
        # print "pn: ", pageNum, " pa: ",pageAddr," pae: ",pageAddrEnd, " len: ",len(page);
        return page;

    def isPageBlank(self, pageNum):
        """Return True if page is blank (ie all bytes set to 0xff), False otherwise"""
        pba=self.getPage(pageNum);
        for b in range(self.pageSize):
            if pba[b] != 0xFF:
                return False;
        return True;

    def set(self, address, bytesA):
        "Throws IndexError if address would exceed size of memory."
        addr=address;
        for i in range(0,len(bytesA)):
            self.memory[addr]=bytesA[i];
            addr+=1;
        
    def loadBinFile(self, filename, startAddress=0):
        ""
        f=open(filename,"rb");
        ba=bytearray(f.read());
        self.set(startAddress,ba);
        

    def dumpToFile(self, filename):
        ""
        f=open(filename, "wb");
        f.write(self.memory);
        f.close()
        

