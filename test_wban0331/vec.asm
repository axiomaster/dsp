   .global  _c_int00
     .sect    ".vecs"

RESET:                    
    MVKL   _c_int00, B0       
    MVKH   _c_int00, B0	
    B      B0             
    xor b1,b1,b2          
    mvc b2,AMR           
    nop               
    nop
    nop
 
NMI:    
    B   IRP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
  

  .space  8*4*2


IE4:
  B		 IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
    
IE5
  B      IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  
IE6   
  B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  
IE7 
  B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP

IE8   
  B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  
IE9  
 B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP 
  
IE10
  B    IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  
IE11
 B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  
IE12
  B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
   
IE13
  B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  
IE14
  B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
   
IE15
 B     IRP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
 
  
