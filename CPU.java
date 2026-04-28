package bcc266TP1.toy;

public class CPU {
	//a cpu pode ter dezenas de registradores
	private int registrador1;
	private int registrador2;
		
	private int PC;
	private Instrucao[] programa;
	private int opcode;
	
	public int getRegistrador1() {
		return registrador1;
	}

	public void setRegistrador1(int registrador1) {
		this.registrador1 = registrador1;
	}

	public int getRegistrador2() {
		return registrador2;
	}

	public void setRegistrador2(int registrador2) {
		this.registrador2 = registrador2;
	}
	
		
	public void setPrograma(Instrucao[] programaAux) {
		programa = programaAux;
	}
	
	public void iniciar(RAM ram) {
		opcode=Integer.MAX_VALUE;
		PC=0;
		//aqui inicia o pipeline
		while(opcode!=-1) {
			//carregar instrucao
			Instrucao inst = programa[PC];
			
			//decodificar instrucao
			opcode = inst.opcode;
			
			//executar instrucao
			//-1 -> halt
			// 0 -> soma
			// 1 -> subtrai
			// 2 -> copia do registrador para RAM
			// 3 -> copia da RAM para o registrador
			switch (opcode){
				//halt
				case -1: {
					System.out.println("programa terminou!!");
					ram.imprimir();
					break;
				}
				//soma
				case 0: {
					registrador1 = ram.getDado(inst.add1);
					registrador2 = ram.getDado(inst.add2);
					registrador1 +=registrador2;
					//salvar resultado
					ram.setDado(inst.add3, registrador1);
					System.out.println("Inst sum -> RAM posicao " + inst.add3 + " com conteudo " + registrador1);
					break;
				}
				//subtrai
				case 1: {
					registrador1 = ram.getDado(inst.add1);
					registrador2 = ram.getDado(inst.add2);
					registrador1 -=registrador2;
					//salvar resultado
					ram.setDado(inst.add3, registrador1);
					System.out.println("Inst sub -> RAM posicao " + inst.add3 + " com conteudo " + registrador1);
					break;
				}
				//copia do registrador para RAM
				//formato da instrucao [opcode,qual_registrador,end_ram,-1]
				case 2: {
					if(inst.add1==1) {
						ram.setDado(inst.add2, registrador1);
						System.out.println("Inst copy_reg_ram -> RAM posicao " + inst.add2 + " com conteudo " + registrador1);
						
					}else if(inst.add1==2) {
						ram.setDado(inst.add2, registrador2);
						System.out.println("Inst copy_reg_ram -> RAM posicao " + inst.add2 + " com conteudo " + registrador2);
						
					}
					break;
				}				
				//copia da RAM para o registrador
				//formato da instrucao [opcode,qual_registrador,end_ram,-1]
				case 3: {
					if(inst.add1==1) {
						registrador1 = ram.getDado(inst.add2);
						System.out.println("Inst copy_ram_reg -> Registrador1 com conteudo " + registrador1);
						
					}else if(inst.add1==2) {
						registrador2 = ram.getDado(inst.add2);
						System.out.println("Inst copy_ram_reg -> Registrador2 com conteudo " + registrador2);
					}
					break;
				}		
			
			}			
			
			PC++;
		}
	}

}
