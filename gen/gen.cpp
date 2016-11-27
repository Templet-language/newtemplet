#pragma templet ~message
#pragma templet ~message =
#pragma templet ~message@(submessage1?,submessage2!,submessage3!,-submessage4?)

#pragma templet *actor
#pragma templet *actor +
#pragma templet *actor@(?,port?message,port!message) +

#pragma templet '~' id ['@'] [ '(' ['-'] id ('!'|'?') {',' ['-'] id ('!'|'?')} ')' ] ['=']
#pragma templet '*' id ['@'] [ '(' ('?'| id ('!'|'?') id) {',' id ('!'|'?') id)} ')' ] ['+']