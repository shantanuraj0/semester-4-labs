package question2_c;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Scanner;

public class Ques_2c {

	public static void main(String[] args) throws SQLException {
		// TODO Auto-generated method stub
		
		Connection connect = null;
		ResultSet resultSet = null;
		
		 connect= DriverManager.getConnection("jdbc:mysql://localhost:3306/assignment1","root" , "raj123SHANPK45");
		 System.out.println("Enter the name of faculty to find courses taught by him: ");
		 Scanner in = new Scanner(System.in);
		 String fname = in.nextLine();
		 
		  
		 PreparedStatement pStmt = connect.prepareStatement("SELECT C.name FROM class C , faculty F WHERE C.fid = F.fid AND fname = ?");
		 pStmt.setString(1, fname); 
		 resultSet = pStmt.executeQuery(); 
			
		 System.out.println("courses: ");
		 while(resultSet.next())
		 {
			 String cname = resultSet.getString("name");
			 
			 
			 System.out.println(" " +cname );
		 }
		 
		 
		 
		 
		 resultSet.close();
		 connect.close();
		 in.close();
		 System.out.println("\nSuccessfully ------done!"); 
	}

}
