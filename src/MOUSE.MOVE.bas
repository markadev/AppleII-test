10  HOME
20  PRINT "This is a demonstration of the mouse"
30  PRINT CHR$(4); "PR#4": PRINT CHR$(1)
40  PRINT CHR$(4); "PR#0"
50  PRINT CHR$(4); "IN#4"
60  INPUT ""; X, Y, S
70  VTAB 10: PRINT X;"  ",Y"  ",S"  "
80  IF S > 0 THEN 60
90  PRINT CHR$(4); "IN#0"
100 PRINT CHR$(4); "PR#4": PRINT CHR$(0)
110 PRINT CHR$(4); "PR#0"
120 END
