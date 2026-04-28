package bcc266TP1.toy;

import java.util.Random;

public class Programas {
	
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		new Programas();
	}
	
	public Programas() {
		RAM ram;
		CPU cpu;
		ram = new RAM();		
		cpu = new CPU();
		
		programaAleatorio(ram, cpu, 2000000);
		//programaMult(ram, cpu, 15, 150);
		//programaDiv(ram, cpu, 175, 4);
		//programaFat(ram, cpu, 10);
		//programaSomaMatriz(ram, cpu, 3);//3x3
		
	}
	
	private void programaAleatorio(RAM ram, CPU cpu, int qdeIntrucoes) {
		Instrucao[] umPrograma = new Instrucao[qdeIntrucoes];
		Random r = new Random();
		int tamanhoRAM = 1000;
		ram.criarRAM_aleatoria(tamanhoRAM);
		for(int i=0; i<(qdeIntrucoes-1); i++) {
			Instrucao inst = new Instrucao();
			inst.opcode = r.nextInt(2);
			inst.add1 = r.nextInt(tamanhoRAM); // quer dizer que a RAM possui 1k posicoes
			inst.add2 = r.nextInt(tamanhoRAM);
			inst.add3 = r.nextInt(tamanhoRAM);
			umPrograma[i] = inst;
		}
		
		Instrucao ultima = new Instrucao();
		ultima.opcode = -1;
		umPrograma[qdeIntrucoes-1] = ultima;
		
		cpu.setPrograma(umPrograma);
		cpu.iniciar(ram);
	}
	
	private void programaSomaMatriz(RAM ram, CPU cpu, int cardinalidade) {
		//gerando as matrizes de forma aleatoria
		int[][] matriz1 = new int[cardinalidade][cardinalidade];
		int[][] matriz2 = new int[cardinalidade][cardinalidade];
		Random r = new Random();
		for(int i=0; i<cardinalidade; i++) {
			for(int j=0; j<cardinalidade; j++) {
				matriz1[i][j] = r.nextInt(100);
				matriz2[i][j] = r.nextInt(100);
			}
		}
		
		//levando tudo pra RAM que é um vetor....
		//9 posicoes para matriz1 + 9 posicoes para matriz2
		//e 9 posicoes para matriz resultante...
		//zerar a RAM
		ram.criarRAM_vazia(27);		
		int endRam=0;
		for (int i=0; i<cardinalidade; i++) {
			for(int j=0; j<cardinalidade; j++) {				
				
				Instrucao[] trecho1 = new Instrucao[2];
				cpu.setRegistrador1(matriz1[i][j]);
				
				Instrucao inst1 = new Instrucao();
				inst1.opcode = 2;
				inst1.add1 = 1;//registrador1
				inst1.add2 = endRam;//ram[endRam]
				trecho1[0] = inst1;
				
				Instrucao inst2 = new Instrucao();
				inst2.opcode = -1;
				trecho1[1] = inst2;
				
				cpu.setPrograma(trecho1);
				cpu.iniciar(ram);
				
				endRam++;
			}			
		}
		
		for (int i=0; i<cardinalidade; i++) {
			for(int j=0; j<cardinalidade; j++) {
				Instrucao[] trecho2 = new Instrucao[2];
				cpu.setRegistrador1(matriz2[i][j]);
											
				Instrucao inst1 = new Instrucao();
				inst1.opcode = 2;
				inst1.add1 = 1;//registrador1
				inst1.add2 = endRam;//ram[endRam]
				trecho2[0] = inst1;
				
				Instrucao inst2 = new Instrucao();
				inst2.opcode = -1;
				trecho2[1] = inst2;
				
				cpu.setPrograma(trecho2);
				cpu.iniciar(ram);
				
				endRam++;
			}			
		}
		endRam=0;
		int delta=9; // valor a ser respeitado entre matriz 1 e 2
		for (int i=0; i<cardinalidade; i++) {
			for(int j=0; j<cardinalidade; j++) {
				Instrucao[] trecho3 = new Instrucao[2];
				Instrucao inst1 = new Instrucao();
				inst1.opcode = 0;//sum
				inst1.add1 = endRam;//ram[endRam]
				inst1.add2 = endRam+delta;//ram[endRam+delta]
				inst1.add3 = endRam+(2*delta);//ram[endRam+(2*delta)]
				trecho3[0] = inst1;
				//[0,...,8, 9, ....,17,  18,19, ...26]						
				Instrucao inst2 = new Instrucao();
				inst2.opcode = -1;
				trecho3[1] = inst2;
				
				cpu.setPrograma(trecho3);
				cpu.iniciar(ram);
				
				endRam++;
			}			
		}
	}
	
	private void programaFat(RAM ram, CPU cpu, int fat) {
		
		//executar instrucao
		//-1 -> halt
		// 0 -> soma
		// 1 -> subtrai
		// 2 -> copia do registrador para RAM
		// 3 -> copia da RAM para o registrador
				
		//10! = 10x9x8x7x6x5x4x3x2x1
		int j=1;
		
		for(int i=1; i<=fat;i++) {
			programaMult(ram, cpu,j,i);
						
			Instrucao[] trecho1 = new Instrucao[2];
			Instrucao inst1 = new Instrucao();
			inst1.opcode = 3;
			inst1.add1 = 1;//registrador1
			inst1.add2 = 0;// RAM[0]
			trecho1[0] = inst1;
			
				
			Instrucao inst2 = new Instrucao();
			inst2.opcode = -1;
			trecho1[1] = inst2;
			
			cpu.setPrograma(trecho1);
			cpu.iniciar(ram);
			
			j = cpu.getRegistrador1();
		}
		
		Instrucao[] trecho2 = new Instrucao[2];
		Instrucao inst3 = new Instrucao();
		inst3.opcode = 3;
		inst3.add1 = 1;//registrador1
		inst3.add2 = 0;//ram[0]
		inst3.add3 = -1;
		trecho2[0] = inst3;
					
		Instrucao inst4 = new Instrucao();
		inst4.opcode = -1;
		trecho2[1] = inst4;
		
		cpu.setPrograma(trecho2);
		cpu.iniciar(ram);
		
		System.out.println("O resultado do fatorial eh: " + cpu.getRegistrador1());
		
	}
	
	private void programaDiv(RAM ram, CPU cpu, int dividendo, int divisor) {
		//zerar a RAM
		ram.criarRAM_vazia(4);
		//Ex. dividir 14 / 3:
		//14-3=11 (1 sub)
		//11-3=8 (2 subs)
		//8-3=5 (3 subs)
		//5-3=2 (4 subs)
		//2-3 < 0 (halt)
		//resultado 4
		
		
		//executar instrucao
		//-1 -> halt
		// 0 -> soma
		// 1 -> subtrai
		// 2 -> copia do registrador para RAM
		// 3 -> copia da RAM para o registrador
				
		Instrucao[] trecho1 = new Instrucao[3];
		cpu.setRegistrador1(dividendo);	
		
		Instrucao inst1 = new Instrucao();
		inst1.opcode = 2;
		inst1.add1 = 1;//registrador1
		inst1.add2 = 0;//ram[0]
		trecho1[0] = inst1;
		
		cpu.setRegistrador2(divisor);
		
		Instrucao inst2 = new Instrucao();
		inst2.opcode = 2;
		inst2.add1 = 2;//registrador2
		inst2.add2 = 1;//ram[1]
		trecho1[1] = inst2;
		
		Instrucao inst3 = new Instrucao();
		inst3.opcode = -1;
		trecho1[2] = inst3;
		
		cpu.setPrograma(trecho1);
		cpu.iniciar(ram);
		
		Instrucao[] trecho2 = new Instrucao[2];
		cpu.setRegistrador1(1);
		
		Instrucao inst4 = new Instrucao();
		inst4.opcode = 2;
		inst4.add1= 1;//registrador1
		inst4.add2 = 2;//ram[2]
		trecho2[0] = inst4;
		
		Instrucao inst5 = new Instrucao();
		inst5.opcode = -1;
		trecho2[1] = inst5;
		
		cpu.setPrograma(trecho2);
		cpu.iniciar(ram);
		
		while(dividendo >= divisor) {
			Instrucao[] trecho3 = new Instrucao[4];
			Instrucao inst6 = new Instrucao();
			inst6.opcode = 1;//sub
			inst6.add1 = 0;//ram[0]
			inst6.add2 = 1;//ram[1]
			inst6.add3 = 0;//ram[0]
			trecho3[0] = inst6;
			
			Instrucao inst7 = new Instrucao();
			inst7.opcode = 0;//sum
			inst7.add1 = 2;//ram[2]
			inst7.add2 = 3;//ram[3]
			inst7.add3 = 3;//ram[3]
			trecho3[1] = inst7;
			
			Instrucao inst8 = new Instrucao();
			inst8.opcode = 3;
			inst8.add1 = 1;//registrador1
			inst8.add2 = 0;//ram[0]
			inst8.add3 = -1;//ram[1]
			trecho3[2] = inst8;
			
						
			Instrucao inst9 = new Instrucao();
			inst9.opcode = -1;
			trecho3[3] = inst9;
			
			cpu.setPrograma(trecho3);
			cpu.iniciar(ram);
			
			dividendo = cpu.getRegistrador1();
		}
		
		Instrucao[] trecho4 = new Instrucao[2];
		Instrucao inst10 = new Instrucao();
		inst10.opcode = 3;
		inst10.add1 = 1;//registrador1
		inst10.add2 = 3;//ram[3]
		inst10.add3 = -1;
		trecho4[0] = inst10;
		
		Instrucao inst11 = new Instrucao();
		inst11.opcode = -1;
		trecho4[1] = inst11;
		
		cpu.setPrograma(trecho4);
		cpu.iniciar(ram);
		
		System.out.println("O resultado da divisao eh: " + cpu.getRegistrador1());
		
	}
	
	
	private void programaMult(RAM ram, CPU cpu, int multiplicando, int multiplicador) {
		//executar instrucao
		//-1 -> halt
		// 0 -> soma
		// 1 -> subtrai
		// 2 -> copia do registrador para RAM
		// 3 -> copia da RAM para o registrador
						
		//zerar a RAM
		ram.criarRAM_vazia(2);
		//Ex. multiplicar 15 x 10, ou seja, 15+15+15... (10vezes)
		Instrucao[] trecho1 = new Instrucao[2];
		cpu.setRegistrador1(multiplicando);	
				
		Instrucao inst1 = new Instrucao();
		inst1.opcode = 2;
		inst1.add1 = 1;//registrador1
		inst1.add2 = 1;//ram[1]
		trecho1[0] = inst1;
		
		Instrucao inst2 = new Instrucao();
		inst2.opcode = -1;
		trecho1[1] = inst2;
		
		cpu.setPrograma(trecho1);
		cpu.iniciar(ram);
		
		for(int i=0; i<multiplicador; i++) {
			Instrucao[] trecho2 = new Instrucao[2];
			Instrucao inst3 = new Instrucao();
			inst3.opcode = 0;
			inst3.add1 = 0;//ram[0]
			inst3.add2 = 1;//ram[1]
			inst3.add3 = 0;//ram[0]
			trecho2[0] = inst3;
			
			Instrucao inst4 = new Instrucao();
			inst4.opcode = -1;
			trecho2[1] = inst4;
			
			cpu.setPrograma(trecho2);
			cpu.iniciar(ram);
		}
		
		Instrucao[] trecho3 = new Instrucao[2];
		Instrucao inst5 = new Instrucao();
		inst5.opcode = 3;
		inst5.add1 = 1;//registrador1
		inst5.add2 = 0;//ram[0]
		inst5.add3 = -1;
		trecho3[0] = inst5;
		
			
		Instrucao inst6 = new Instrucao();
		inst6.opcode = -1;
		trecho3[1] = inst6;
		
		cpu.setPrograma(trecho3);
		cpu.iniciar(ram);
		
		System.out.println("O resultado da multiplicacao eh: " + cpu.getRegistrador1());
		
	}

}
