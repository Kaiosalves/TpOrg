package bcc266TP2.toy;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
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
		programaAleatorio(ram, cpu, 1000, 100);
		//programaAleatorioRepeticoes(ram, cpu);
	}
	
	void programaAleatorioRepeticoes(RAM ram, CPU cpu) {
		
		Instrucao[] trecho1 = new Instrucao[10001];
		
		//ler do arquivo uma linha
		try{
			File f = new File ("programa.txt");
			FileReader fis = new FileReader(f);
			BufferedReader br = new BufferedReader(fis);
			
			String linha = null;
			Instrucao umaInstrucao = null;
			int index=0;
			
			while ((linha = br.readLine())!=null){
				String [] palavras = linha.split(":");
				umaInstrucao = new Instrucao();
				
				umaInstrucao.opcode = Integer.parseInt(palavras[0]);
				
				Endereco e1 = new Endereco();
				int e1_endBloco = Integer.parseInt(palavras[1]);
				int e1_endPalavra = Integer.parseInt(palavras[2]);
				e1_endPalavra = e1_endPalavra%4; //mod 4
				e1.endBloco = e1_endBloco;
				e1.endPalavra = e1_endPalavra;
				umaInstrucao.add1 = e1;
				
				Endereco e2 = new Endereco();
				int e2_endBloco = Integer.parseInt(palavras[3]);
				int e2_endPalavra = Integer.parseInt(palavras[4]);
				e2_endPalavra = e2_endPalavra%4; //mod 4
				e2.endBloco = e2_endBloco;
				e2.endPalavra = e2_endPalavra;
				umaInstrucao.add2 = e2;
				
				Endereco e3 = new Endereco();
				int e3_endBloco = Integer.parseInt(palavras[5]);
				int e3_endPalavra = Integer.parseInt(palavras[6]);
				e3_endPalavra = e3_endPalavra%4; //mod 4
				e3.endBloco = e3_endBloco;
				e3.endPalavra = e3_endPalavra;
				umaInstrucao.add3 = e3;
				
				trecho1[index] = umaInstrucao;
				index++;
			}
			fis.close();
			br.close();
			
			//inserindo a ultima instrucao do programa que faz o halt
			umaInstrucao = new Instrucao();
			umaInstrucao.opcode = -1;
						
			trecho1[10000] = umaInstrucao;
			
			ram.criarRAM_aleatoria(1000);
			cpu.setPrograma(trecho1);
			cpu.iniciar(ram, 16, 32);
			
		}catch (Exception e){
			e.printStackTrace();
		}
	}
	
	void programaAleatorio(RAM ram, CPU cpu, int qdeInstrucoes, int tamanhoRam) {
		
		Instrucao[] trecho1 = new Instrucao[qdeInstrucoes];
		
		Instrucao umaInstrucao;
		
		Random r = new Random();
		for (int i=0; i<qdeInstrucoes-1; i++){	
			
			umaInstrucao = new Instrucao();
			umaInstrucao.opcode = r.nextInt(2);
			
			Endereco add1 = new Endereco();
			add1.endBloco = r.nextInt(tamanhoRam);
			add1.endPalavra = r.nextInt(4);
			umaInstrucao.add1 = add1;
			
			Endereco add2 = new Endereco();
			add2.endBloco = r.nextInt(tamanhoRam);
			add2.endPalavra = r.nextInt(4);
			umaInstrucao.add2 = add2;
			
			Endereco add3 = new Endereco();
			add3.endBloco = r.nextInt(tamanhoRam);
			add3.endPalavra = r.nextInt(4);
			umaInstrucao.add3 = add3;
			
			trecho1[i] = umaInstrucao;
		}
		
		//inserindo a ultima instrucao do programa que faz o halt
		umaInstrucao = new Instrucao();
		umaInstrucao.opcode = -1;
					
		trecho1[qdeInstrucoes-1] = umaInstrucao;
		
		ram.criarRAM_aleatoria(tamanhoRam);
		cpu.setPrograma(trecho1);
		cpu.iniciar(ram, 32, 64);
	}

}
