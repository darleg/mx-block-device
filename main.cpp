#include "mbed.h"
#include "BlockDevice.h"
#include <stdio.h>
#include <algorithm>

#define TX PA_11
#define RX PA_12

static BufferedSerial az(TX, RX);

FileHandle *mbed::mbed_override_console(int fd)
{
    return &az;
}

// System's default block device.
BlockDevice *aBlock = BlockDevice::get_default_instance();

// main() runs in its own thread in the OS
int main()
{
    char wifissid[] = "SSID";
    char wifipsw[] = "password";
    
     // Initialize the block device.
    int a = aBlock->init();

    // Get device geometry aBlock.
    bd_size_t aReadSize    = aBlock->get_read_size();
    bd_size_t aProgramSize = aBlock->get_program_size();
    bd_size_t aEraseSize   = aBlock->get_erase_size();
    bd_size_t aSize        = aBlock->size();

    printf("aEraseSize:   %lld B\n", aEraseSize);

    //Allocate a block for wifissid
    size_t wiBufferSize = sizeof(wifissid) + aProgramSize - 1;
    wiBufferSize = wiBufferSize - (wiBufferSize % aProgramSize);
    char *wiBuffer = new char[wiBufferSize];
    //Allocate a block for wifipassword
    size_t pasBufferSize = sizeof(wifipsw) + aProgramSize - 1;
    pasBufferSize = pasBufferSize - (pasBufferSize % aProgramSize);
    char *pasBuffer = new char[pasBufferSize];
    
    // Update the buffer with the string we want to store.
    strncpy(wiBuffer, wifissid, wiBufferSize);
    strncpy(pasBuffer, wifipsw, pasBufferSize);
    // Write data to the first block.
    a = aBlock->erase(0, aEraseSize);
    a = aBlock->program(wiBuffer, 0, wiBufferSize);
    // Write data to the second block.
    int nAddress = aEraseSize + 1;
    printf("nAddress: %d\n", nAddress);
    a = aBlock->erase(nAddress, aEraseSize);
    a = aBlock->program(pasBuffer, nAddress, pasBufferSize);

    // Clobber the buffer so we don't get old data.
    memset(wiBuffer, 0xcc, wiBufferSize);
    memset(pasBuffer, 0xcc, pasBufferSize);

    a = aBlock->read(wiBuffer, 0, wiBufferSize);
    a = aBlock->read(pasBuffer, nAddress, pasBufferSize);

    printf("--- Stored data ---\n");  
    printf(" %.*s\n", wiBufferSize, &wiBuffer[0]);
    printf(" %.*s\n", pasBufferSize, &pasBuffer[0]);
    printf("---\n");

    // Deinitialize the block device.
    a = aBlock->deinit();

    while (true) {

    }
}

