

#include <kernel.h>  



uint8_t hdd_ata_status_read (unsigned short port)
{
    if (port<0){
        panic("hdd_ata_status_read: p");
    }

// #bugbug: 
// Rever o offset.

    return (uint8_t) in8( (unsigned short) (ide_ports[port].base_port + 7) );
}


int hdd_ata_wait_not_busy (unsigned short port)
{
    while ( hdd_ata_status_read(port) & ATA_SR_BSY )
    {
        if ( hdd_ata_status_read(port) & ATA_SR_ERR )
        {
            return TRUE;
        }
    };

    return FALSE;
}

void hdd_ata_cmd_write ( unsigned short port, unsigned char cmd_val )
{
    if (port<0)
        panic("hdd_ata_status_read: port");

    // no_busy 
    hdd_ata_wait_not_busy (port);

    //outb(ata.cmd_block_base_address + ATA_REG_CMD,cmd_val);
    
    out8 ( 
        (unsigned short) (ide_ports[port].base_port + 7), 
        (unsigned char) cmd_val );

    ata_wait (400);  
}

int hdd_ata_wait_no_drq (unsigned short port)
{
    while ( hdd_ata_status_read(port) & ATA_SR_DRQ)
    {
        if (hdd_ata_status_read(port) & ATA_SR_ERR)
        {
            return TRUE;
        }
    };

    return FALSE;
}


// #todo: buffer address 64bit
static void hdd_ata_pio_read ( unsigned short port, void *buffer, int bytes )
{

// #todo
// Se possível, invocar a rotina padrão para portas.
// See: ports64.c

    //#todo
    //if ( (void*) buffer == NULL ){ return; );

    asm volatile (\
        "cld;\
        rep; insw":: "D" (buffer),\
        "d" ( (unsigned short) (ide_ports[port].base_port + 0) ),\
        "c" (bytes/2) );
}


// #todo: buffer address 64bit
void hdd_ata_pio_write ( unsigned short port, void *buffer, int bytes )
{

// #todo
// Se possível, invocar a rotina padrão para portas.
// See: ports64.c

    //#todo
    //if ( (void*) buffer == NULL ){ return; );
    
    asm volatile (\
        "cld;\
        rep; outsw"::"S" (buffer),\
        "d" ( (unsigned short) (ide_ports[port].base_port + 0) ),\
        "c" (bytes/2));
}


/*
 *******************************
 * pio_rw_sector:
 * 
 * IN:
 *   buffer - Buffer address. ??? virtual address ??
 *   lba - LBA number 
 *   rw - Flag read or write.
 *
 *   //inline unsigned char in8 (int port)
 *   //out8 ( int port, int data )
 *   (IDE PIO)
 */

// # Changing the lba type to 'unsigned int' 32 bit.

int 
pio_rw_sector ( 
    unsigned long buffer, 
    unsigned int _lba, 
    int rw, 
    unsigned short port,
    int slave )
{
    unsigned char c=0;
    unsigned int lba = (unsigned int) _lba;


    if (rw<0)
    {
         // msg?
         return (-1);
    }

// #bugbug> unsigned short?

    port = (unsigned short) (port & 0xFFFF);

    if ( port < 0 || port >= 4 )
    {
         // msg?
         return (-1);
    }

    if (slave<0)
    {
         // msg?
         return (-1);
    }


// #todo
// Maybe check the limits for lba.
// Based on the current disk's properties.

// =========================

	//Selecionar se é master ou slave.
	//outb (0x1F6, slavebit<<4)
	//0 - 3     In CHS addressing, bits 0 to 3 of the head. 
	//          In LBA addressing, bits 24 to 27 of the block number
	//4 DRV Selects the drive number.
	//5 1   Always set.
	//6 LBA Uses CHS addressing if clear or LBA addressing if set.
	//7 1   Always set.


// #todo
// O número da porta já indica se o dispositivo é 
// master ou slave.
// Nesse caso nem precisamos do parâmetro 'slave'.
// #todo
// A intenção aqui é não usar mais o parametro 'slave'

// See:
// ide_ports[port].channel
// ide_ports[port].dev_num

// #debug
    if ( ide_ports[port].dev_num != slave )
    {
        panic("pio_rw_sector: [DEBUG] WRONG PARAMETER\n");
    }


// =================================================
// 0x01F6 
// Port to send drive and bit 24 - 27 of LBA
// Setup the bit 24-27 of the lba.

    lba = (int) (_lba >> 24);

// no bit 4.
// 0 = master 
// 1 = slave
// master. bit 4 = 0
// 1110 0000b;
// slave. bit 4 = 1
//1111 0000b;


// not slave
    if (slave == 0){ 
        lba = (unsigned int) (lba | 0x000000E0);
    }

// slave
    if (slave == 1){
        lba = (unsigned int) (lba | 0x000000F0);
    }

    out8 ( 
        (unsigned short) (ide_ports[port].base_port + 6), 
        (unsigned char) lba );

// =================================================
// 0x01F2: 
// Port to send, number of sectors
    
    out8 ( 
        (unsigned short) (ide_ports[port].base_port + 2), 
        (unsigned char) 1 );

// =================================================
// 0x1F3: 
// Port to send, bit 0 - 7 of LBA

    lba = (unsigned int) _lba;
    lba = (unsigned int) (lba & 0x000000FF);
    out8 ( 
        (unsigned short) (ide_ports[port].base_port + 3), 
        (unsigned char) lba );


// =================================================
// 0x1F4: 
// Port to send, bit 8 - 15 of LBA

    lba = (unsigned int) _lba;
    lba = (unsigned int) (lba >> 8);
    lba = (unsigned int) (lba & 0x000000FF);
    out8 ( 
        (unsigned short) (ide_ports[port].base_port + 4), 
        (unsigned char) lba );


// =================================================
// 0x1F5  ; Port to send, bit 16 - 23 of LBA

    lba = (unsigned int) _lba;
    lba = (unsigned int) (lba >> 16);
    lba = (unsigned int) (lba & 0x000000FF);
    out8 ( 
        (unsigned short) (ide_ports[port].base_port + 5), 
        (unsigned char) lba );

// =================================================
// 0x1F7; Command port
// rw

    rw = (int) (rw & 0x000000FF);
    out8 ( 
        (unsigned short) (ide_ports[port].base_port + 7), 
        (unsigned char) rw );


// PIO or DMA ??
// If the command is going to use DMA, 
// set the Features Register to 1, otherwise 0 for PIO.

    //out8 (0x1F1, isDMA)


//
// # timeout sim, 
// não podemos esperar para sempre.
//


// #bugbug:
// Isso deve ir para cima.

    unsigned long timeout = (5000*512);

//++
// =========================
again:

// Pega um byte de status.
    c = (unsigned char) in8 ( (unsigned short) ide_ports[port].base_port + 7 );

// Seleciona o bit do byte de status.
    c = (unsigned char) ( c & 8 );

// Checa o estado do bit.
    if ( c == 0 )
    {
        timeout--;

        if ( timeout == 0 )
        {
            printf ("pio_rw_sector: rw sector timeout fail\n");
            // refresh_screen(); ??
            return -3;
        }

        // #bugbug: 
        // Isso pode enrroscar aqui.
        goto again;
    }
// ================================
//--


//
// Read or write.
//

    switch (rw){

// read
// #todo: buffer address 64bit

    case 0x20:
        // read
        hdd_ata_pio_read ( 
            (unsigned short) port, 
            (void *) buffer, 
            (int) 512 );
        return 0;
        break;

// write
// #todo: buffer address 64bit

    case 0x30:
        // write
        hdd_ata_pio_write ( 
            (unsigned short) port, 
            (void *) buffer, 
            (int) 512 );

        //Flush Cache
        hdd_ata_cmd_write ( 
            (unsigned short) port, 
            (unsigned char) ATA_CMD_FLUSH_CACHE );
 
        hdd_ata_wait_not_busy (port);
        if ( hdd_ata_wait_no_drq(port) != 0)
        {
            // msg?
            return -1;
        }
        return 0;
        break;

// fail
    default:
        panic ("pio_rw_sector: fail *hang");
        break; 
    };

    return 0;
}


/*
 *****************************************
 * ataReadSector:
 * 
 * buffer - buffer
 * lba - lba
 * reserved1 - null
 * reserved2 - null
 * 
 */
 
int
ataReadSector ( 
    unsigned long buffer, 
    unsigned long lba, 
    unsigned long reserved1, 
    unsigned long reserved2 )
{

    int Status=0;

// ======================== ATENÇAO ==============================
// #IMPORTANTE:
// #todo
// Só falta conseguirmos as variáveis que indicam o canal e 
// se é master ou slave.


/*
// ====================================================
// #test
// lba limits

// #todo
// Check limits for 'buffer' and 'lba'
// The lba limits is given by the disk properties.
// For now all we have is a 32mb virtual disk.
// Our limit is (32*1024*1024)/512
// #todo
// But we need to use the ata driver to find the size
// of the disk when running the system on a real HD.
// In this case the size is bigger than the
// size of the virtual disk.
// #todo
// Cada disco tem que ter seu limite.

    unsigned long vhd_lba_limits=0;

// #bugbug
// Nao podemos calcular isso toda vez que chamarmos essa rotina,
    vhd_lba_limits = (unsigned long) ((32*1024*1024)/512);

    if ( lba >= vhd_lba_limits )
        panic("ataReadSector> lba limits");

// ====================================================
*/

    unsigned short port = (unsigned short) g_current_ide_channel;
    int is_master = (int) g_current_ide_device;

    if( is_master != 0 && is_master != 1 )
        panic("ataReadSector: is_master");

// IN:
// (buffer, lba, rw flag, port number, master )

    Status = pio_rw_sector ( 
        (unsigned long) buffer, 
        (unsigned long) lba, 
        (int) 0x20,
        (unsigned short) port,  // 0 ... channel
        (int) is_master );      // 1 ... master=1 slave=0

    return Status;
}


/*
 *************************************
 * ataWriteSector:
 * 
 * buffer - buffer
 * lba - lba
 * reserved1 - null
 * reserved2 - null
 */

int
ataWriteSector ( 
    unsigned long buffer,
    unsigned long lba,
    unsigned long reserved1,
    unsigned long reserved2 )
{

    int Status=0;

// ================ ATENÇAO ==============================
// #IMPORTANTE:
// #todo
// Só falta conseguirmos as variáveis que indicam o canal e 
// se é master ou slave.


// #todo
// Limits for 'buffer' and 'lba'.


    unsigned short port = (unsigned short) g_current_ide_channel;
    int is_master = (int) g_current_ide_device;

    if( is_master != 0 && is_master != 1 )
        panic("ataWriteSector: is_master");

// IN:
// (buffer, lba, rw flag, port number, master )

    Status = pio_rw_sector ( 
        (unsigned long) buffer, 
        (unsigned long) lba, 
        (int) 0x30, 
        (unsigned short) port,  // 0 ... channel
        (int) is_master );      // 1 ... master=1 slave=0


    return Status;
}


/*
 ***************************************
 * init_hdd:
 *     Inicializa o driver de hd.
 *     @todo: Mudar para hddInit().
 */

int init_hdd (void)
{

	// #todo: 
	// We need to do something here.

    g_driver_hdd_initialized = (int) TRUE;
    return 0;
}















