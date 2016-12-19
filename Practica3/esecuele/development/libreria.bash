COMMAND="./esecuele"

rm -rf libros_db

echo "Creando libros_db"
$COMMAND createdb libros_db

echo "Creando tablas"
$COMMAND define libros_db << EOF
TABLE edicion 7 STR STR STR STR STR STR STR
TABLE compra 5 STR LNG STR STR INT
TABLE usuario 5 LNG STR STR STR STR STR
EOF

echo "Insertando datos en tablas"
$COMMAND insert libros_db << EOF
COPY edicion ./example_files/edicion.txt
COPY compra ./example_files/compra.txt
COPY usuario ./example_files/usuario.txt
EOF

## echo "Título de los libros comprados por jack hecho por David:"
## $COMMAND query libros_db << EOF
## usuario SEQUENTIAL 2 STR jack C_COLEQCTE SELECT compra SEQUENTIAL PRODUCT 0 6 C_COLEQCOL SELECT STR 5 P_COL 1 PROJECT edicion SEQUENTIAL PRODUCT 0 1 C_COLEQCOL SELECT STR 6 P_COL 1 PROJECT
## EOF
## echo  "Número de libros comprados por jack hecho por David:"
## $COMMAND query libros_db << EOF
## usuario SEQUENTIAL 2 STR jack C_COLEQCTE SELECT compra SEQUENTIAL PRODUCT 0 6 C_COLEQCOL SELECT COUNT
## EOF

echo "Título de los libros comprados por jack:"
$COMMAND query libros_db << EOF
compra SEQUENTIAL usuario SEQUENTIAL 2 STR jack C_COLEQCTE SELECT LNG 0 P_COL 1 PROJECT PRODUCT 1 5 C_COLEQCOL SELECT STR 0 P_COL 1 PROJECT edicion SEQUENTIAL STR 0 P_COL STR 5 P_COL 2 PROJECT PRODUCT 0 1 C_COLEQCOL SELECT STR 2 P_COL 1 PROJECT
EOF
echo  "Número de libros comprados por jack:"
$COMMAND query libros_db << EOF
compra SEQUENTIAL usuario SEQUENTIAL 2 STR jack C_COLEQCTE SELECT LNG 0 P_COL 1 PROJECT PRODUCT 1 5 C_COLEQCOL SELECT COUNT
EOF
