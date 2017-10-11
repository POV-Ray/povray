'============================================================================
' ArgV - gibt einen bestimmten Parameter aus der Kommandozeile zurÅck.
'
' Which = Nummer des Parameters
' Cmd   = COMMAND$, oder ein String von Parametern
'
FUNCTION ArgV(BYVAL Which AS INTEGER, BYVAL Cmd AS STRING) PUBLIC AS STRING

  DIM arg AS INTEGER
  DIM f   AS STRING
  DIM q   AS INTEGER

  DO WHILE LEN(cmd)
    INCR arg
    f = LEFT$(cmd, 1)
    IF ASCII(f) = 34 THEN
      q = INSTR(MID$(cmd,2), CHR$(34))
      IF q THEN
        f = LEFT$(cmd, q+1)
      ELSE
        f = cmd
      END IF
    ELSE
      f = f + EXTRACT$(MID$(cmd,2), ANY CHR$(34)+" /")
    END IF
    cmd = LTRIM$(MID$(cmd, len(f)+1))
    IF arg = which THEN
      EXIT DO
    ELSE
      f = ""
    END IF
  LOOP

  ArgV = f

END FUNCTION
