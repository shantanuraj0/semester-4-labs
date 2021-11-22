package question2_b;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Scanner;

public class Ques_2b {

	public static void main(String[] args) throws SQLException {
		// TODO Auto-generated method stub
		
		Connection connect = null;
		ResultSet resultSet = null;
		
		 connect= DriverManager.getConnection("jdbc:mysql://localhost:3306/assignment1","root" , "raj123SHANPK45");
		 System.out.println("Enter the course name to find its room no and timing : ");
		 Scanner in = new Scanner(System.in);
		 String cname = in.nextLine();
	
		 
		 
		    PreparedStatement pStmt = connect.prepareStatement("SELECT room , meets_at FROM class WHERE name = ?");
			pStmt.setString(1, cname); 
			resultSet = pStmt.executeQuery();  
		    
		 System.out.printf("%-40s %-40s\n", "room", "meets_at");
		 while(resultSet.next())
		 {
			 System.out.printf("%-40s ",resultSet.getString("room"));
			 System.out.printf("%-40s\n",resultSet.getString("meets_at"));
		 }
		 
		 
		 
		 
		 resultSet.close();
		 //statement.close();
		 connect.close();
		 in.close();
		 System.out.println("\nSuccessfully ------done!"); 
		
		
		
	}

}
