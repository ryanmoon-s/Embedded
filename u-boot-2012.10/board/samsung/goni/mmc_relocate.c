typedef unsigned int (*copy_sd_mmc_to_mem) (unsigned int  channel, unsigned int  start_block, unsigned char block_size, unsigned int  *trg, unsigned int  init);


void copy_code_to_dram(void)
{
	unsigned long ch;
	unsigned long dest = 0x34800000;  //加载的目标地址
	unsigned int sec_no = 49;  //扇区初始地址

	ch = *(volatile unsigned int *)(0xD0037488);

	// 函数指针
	copy_sd_mmc_to_mem copy_bl2 = (copy_sd_mmc_to_mem) (*(unsigned int *) (0xD0037F98));

	unsigned int ret;
	
	// 通道0
	if (ch == 0xEB000000)
	{
		// 0:channel 0
		// 49:源代码位于扇区49,1 sector = 512 bytes
		// 32:长度，拷贝32 sector，即16K
		// 0x23E00000:目的,链接地址0x23E00000
		ret = copy_bl2(0, sec_no,       128, (unsigned int *)dest, 0);  //64k
		ret = copy_bl2(0, sec_no + 128, 128, (unsigned int *)(dest+ 0x10000), 0);  //64k
		ret = copy_bl2(0, sec_no + 256, 128, (unsigned int *)(dest + 0x20000), 0);    //64k
	}
}
