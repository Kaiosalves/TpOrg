package large_vector;

public class MainAppl {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		new MainAppl();
	}
	
	public MainAppl(){
		LargeVector<Integer> vet1 = new LargeVector<Integer>();
		vet1.initialize(1087654390);
		for(int i=0; i<1087654390; i++)
			vet1.put(i, i);
		
		for(int i=0;i<1087654390;i++)
			System.out.println("pos "+ i + " content: " + vet1.get(i));
	}

}
