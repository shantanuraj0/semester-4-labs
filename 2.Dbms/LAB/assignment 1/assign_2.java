
import java.io.BufferedReader;  
import java.io.FileReader;  
import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.util.Scanner; // Import the Scanner class to read text files

public class assign_2
{
  public static void main(String[] args) throws Exception
  {
	int count=0 ;
	
    try 
	{
	  String line;
	  Scanner s1 = new Scanner(System.in);
	  System.out.println("Enter department id: ");
	  String f = s1.next();
	  
	  
	  FileReader file = new FileReader("Employee.txt");  
      BufferedReader br = new BufferedReader(file);  
              
        //Gets each line till end of file is reached  
        while((line = br.readLine()) != null) {  
            //Splits each line into words  
            String words[] = line.split(" ");  
           if(words[3].equalsIgnoreCase(f))
		   {	count++;
			   System.out.println(words[1]);
		   }
             
        } 

		br.close();
      
    } 
	
	catch (FileNotFoundException e) 
	{
      System.out.println("An error occurred.");
      e.printStackTrace();
    }
	System.out.println(count++);
  }
}