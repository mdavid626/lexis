/' Zajisteni zakladni kompatibility IFJ10->FreeBASIC '/
#Lang "deprecated" /' Starsi dialekt BASICu '/
Option Escape /' Aktivuje escape sekvence '/

Function sort (s As String) As String
/' Bubble Sort '/
Dim i, j, n, finished As Integer
Dim _swap As Byte
Dim result As String
  result = s
  n = Len(result)
  i = 1
  Do
    finished = 1
    For j = n - 1 To i Step -1
      If result[j-1] > result[j] Then
        _swap = result[j-1]
        result[j-1] = result[j]
        result[j] = _swap
        finished = 0
      End If
    Next j
    i += 1
  Loop Until (finished or (i = n + 1))
  Return result
End Function

Function find (haystack As String, needle As String) As Integer
  Return InStr(haystack, needle)
End Function

/' Program 1: Vypocet faktorialu (iterativne) '/
/' Vcetne ukazky case-insensitive vlastnosti jazyka IFJ10 '/
scope 'Hlavni telo programu
  Dim a As Integer
  DIM vysl AS INTEGER
  
  PrinT "Zadejte cislo pro vypocet faktorialu";
  InpuT ; A
  If a < 0 THEN
    print "\nFaktorial nelze spocitat\n";
  ELSE
    Vysl = 1
    Do WHile A > 0
      VYSL = vysl * a
      a = A - 1
    LooP
    Print "\nVysledek je:" ; vYsl ; "\n";
  end IF
END SCOPE
