ENTRY(main)
SECTIONS
{
    . = 0x80EAE7E0;
    .text :
    {
        *(.main);
        *(.text*);
    }

    . = 0x80EB5000;
    .text :
    {
        *(.exceptions);
    }
}