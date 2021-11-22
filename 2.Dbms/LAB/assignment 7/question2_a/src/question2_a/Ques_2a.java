package question2_a;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Scanner;

public class Ques_2a {

	public static void main(String[] args) throws SQLException {
		// TODO Auto-generated method stub
		Connection connect = null;
		
		
		ResultSet resultSet = null;
		
		 connect= DriverManager.getConnection("jdbc:mysql://localhost:3306/assignment1","root" , "raj123SHANPK45");
		 System.out.println("Enter the name of room  to find its classes: ");
		 Scanner in = new Scanner(System.in);
		 String room = in.nextLine();
		 
		 
		 	PreparedStatement pStmt = connect.prepareStatement("SELECT name FROM class WHERE room = ?");
			pStmt.setString(1,room); 
			resultSet = pStmt.executeQuery();  
		    
			System.out.println("Classes conducted in "+room+":");
			
		 while(resultSet.next())
		 {
			 String cname = resultSet.getString("name");
			 System.out.println(" " + cname);
		 }
		 
		 
		 
		 
		 resultSet.close();
		 pStmt.close();
		 connect.close();
		 in.close();
		 System.out.println("\nSuccessfully ------done!"); 
	 	 
	}

}
