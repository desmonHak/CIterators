# CIterators

----

## Perfil modo debug:

1. Limpiar el directorio actual:
```
mingw32-make -f windows.mk cleanall
```

2. Compilar ejemplos en modo debug:
```
mingw32-make -f windows.mk examples MODE_GEN_LIB=debug
```

----

## Perfil modo release

1. Limpiar el directorio actual:
```
mingw32-make -f windows.mk cleanall
```

2. Compilar ejemplos en modo release:
```
mingw32-make -f windows.mk examples MODE_GEN_LIB=release
```

----

## Perfil modo gprof

1. Limpiar el directorio actual:
```
mingw32-make -f windows.mk cleanall
```

2. Compilar ejemplos en modo release:
```
mingw32-make -f windows.mk examples MODE_GEN_LIB=gprof
```

----