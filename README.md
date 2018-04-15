# VendingMachine.X
Vending Machine 5 lines and 8 collumns


Draw flow chart

------ FRAME ------

| byte 01 | byte 02 | byte 03 | byte 04 | byte 05 | byte 06 | byte 07 | byte 08 | byte 09 |

|-Inicial-|-Propri.-|-Endereço|-Valor---------------------------------|-CRC---------------|

	Byte Inicial = 0x83 
		Determina que esse é o começo da menssagem é conhecida.

	Byte Propriedade
	
		Propriedade SET = 0x00
			Usada para atribuir um valor a tal funcionalidade
		Propriedade GET = 01
			Usada para receber um valor
		Propriedade ACK = 0x02
			Usada pelo equipamento como resposta afirmativa ao comando executado	
 		Propriedade Run = 0x03
			Usada pelo equipamento como indicação que comando foi recebido e será executado
		Propriedade Falha = 0xFF
			Usada pelo equipamento como resposta negativa a uma menssagem com falha
			Byte Inicial Errado
			CRC Errado
		Propriedade Desconhecida = 0xFD
			Usada pelo equipamento como resposta negativa a uma menssagem correta porem com Endereço ou Valor Errado
			ou no caso de ocorrer alguma falha na execução do comando

O valor deve ser usado 4 bytes ou um unsigned int de 64bits (little-endian)
Endereço e valores:

	Endereço Sensor de presensa = 0x10
		Responde apenas propriedade de GET;
		byte 07/06/05/04 = 0x00;

	Endereço Sensor Dispenser = 0x11
		Responde apenas propriedade de GET
		byte 07/06/05/04 = 0x00;
	------
	Endereço RGB = 0x20
		Responde apenas a propriedade de SET;
		byte 07 (0 - 255) referente ao Red do RGB;
		byte 06 (0 - 255) referente ao Green do RGB;
		byte 05 (0 - 255) referente ao Blue do RGB;
		byte 04 = 0x00;

	Endereço Trava da porta = 0x21
		Responde apenas a propriedade de SET;
		byte 07/06/05/04 = Tempo em Segundos que devera ficar aberto;

	Endereço Alinhamento dos motores = 0x22
		Responde apenas a propriedade de SET;
		  byte 07 = 0x01;
		  byte 06/05/04 = 0x00;

	Endereço Atuação Motor unico = 0x23
		Responde apenas a propriedade de SET;
		byte 07 (0 - 4) referente a linha
		byte 06 (0 - 7) referente a coluna
		byte 05/04 = 0x00

	Endereço Atuação Motor duplo = 0x24
		Responde apenas a propriedade de SET;
		byte 07 (0 - 4) referente a linha
		byte 06 (0 - 7) referente a primeira coluna
		byte 05 (0 - 7) referente a segunda coluna
		byte 04 = 0x00;	

	Endereço ativa o Envio sensor de presensa a cada 500ms = 0x25
		Responde apenas a propriedade de SET;
		byte 07 = 0x01;
		byte 06/05/04 = 0x00;
	-------
	Endereço teste para retorno do elevador = 0x30
		byte 07/06/05/04 = 0x00;

	Endereço teste para decida do elevador até o limite = 0x31
		byte 07/06/05/04 = 0x00

	Endereço teste para decida do elevador até a linha deteminada = 0x32
		byte 07 (0 - 4) referente a linha
		byte 06/05/04 = 0x00

	Endereço teste para acionamento de mola unica = 0x33	
		byte 07 (0 - 4) referente a linha
		byte 06 (0 - 7) referente a coluna
		byte 05/04 = 0x00

	Endereço teste para Acionamento de mola dupla = 0x34
		byte 07 (0 - 4) referente a linha
		byte 06 (0 - 7) referente a primeira coluna
		byte 05 (0 - 7) referente a segunda coluna
		byte 04 = 0x00;	

	Endereço teste para Abertura do dispenser = 0x35
		byte 07/06/05/04 = 0x00;

	Endereço teste para fechamento do dispenser = 0x36
		byte 07/06/05/04 = 0x00;

	Endereço teste para alinhamento das molas = 0x37
		byte 07/06/05/04 = 0x00;

CRC
	O crc deve ser calculado usando todos os bytes (de 1 a 7);
	O crc usado é o CRC-CCITT (0xFFFF) (16 bits).

	O algoritimo pode ser encontrado no link abaixo:
	https://www.lammertbies.nl/comm/info/crc-calculation.html
