



import java.io.BufferedReader;  
import java.io.FileReader;  
import java.io.File;  
import java.io.FileNotFoundException;  
import java.util.Scanner; 

public class assign_3
{
  public static void main(String[] args) throws Exception
  {
	
	
    try 
	{
	  String line,line1;
	  Scanner s1 = new Scanner(System.in);
	  System.out.println("Enter Employee id : ");
	  String f = s1.next();
	  String depart_id =null;
	  
	  
	  FileReader file = new FileReader("Employee.txt");  
      BufferedReader br = new BufferedReader(file);  
                
        while((line = br.readLine()) != null) {  
            String words[] = line.split(" ");  
           if(words[0].equalsIgnoreCase(f))
		   {	
			   depart_id = words[3];
			   break;
		   }
             
        } 
		
		
		FileReader file1 = new FileReader("Department.txt");  
		BufferedReader br1 = new BufferedReader(file1);  
              
        
        while((line1 = br1.readLine()) != null) {  
            String words[] = line1.split(" ");  
           if(words[0].equalsIgnoreCase(depart_id))
		   {	
			   
			   System.out.println(words[2]);
			   
		   }
             
        } 
      br.close();
	  br1.close();
    } 
	
	catch (FileNotFoundException e) 
	{
      System.out.println("An error occurred.");
      e.printStackTrace();
    }
	
  }
}