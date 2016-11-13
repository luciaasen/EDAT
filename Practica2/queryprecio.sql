-- Este código es un desglose de todos los casos que se me han ocurrido a la
-- hora de conseguir una tabla igual que compra, pero que contenga la columna
-- precio final. Probablemente haya una manera de hacerlo sin tantísimas tablas
-- y de manera más simplificada pero creo que una tabla que contenga estas 6 vale.

-- Compras que han hecho usuarios no fidelizados en oferta

SELECT C.isbn, C.nousuario, C.metodo, C.fecha, (E.precio * O.descuento) as precio_no_fid_oferta
FROM public.compra as C, public.afectaoferta as A, public.oferta as O, public.edicion as E
WHERE C.nousuario = 00000 and O.id = A.ofertaid and A.isbn = C.isbn
    and C.fecha <= O.fecha2 and C.fecha >= O.fecha1 and E.isbn = C.isbn

-- Compras que han hecho usuarios no fidelizados sin estar en oferta en ese momento (pero alguna vez sí)

SELECT C.isbn, C.nousuario, C.metodo, C.fecha, E.precio
FROM public.compra as C, public.afectaoferta as A, public.oferta as O
WHERE C.nousuario = 00000 and O.id = A.ofertaid and A.isbn = C.isbn
    and C.fecha > O.fecha2 and C.fecha < O.fecha1 and E.isbn = C.isbn

-- Compras que han hecho usuarios no fidelizados de libros que nunca han estado en oferta

SELECT C.isbn, C.nousuario, C.metodo, C.fecha, E.precio
FROM public.compra as C, public.afectaoferta as A, public.edicion as E
WHERE C.nousuario = 00000 and O.id not in public.afectaoferta and E.isbn = C.isbn

-- Compras que han hecho usuarios fidelizados en oferta

SELECT C.isbn, C.nousuario, C.metodo, C.fecha, (E.precio * O.descuento * 0,9) as precio_fid_oferta
FROM public.compra as C, public.afectaoferta as A, public.oferta as O, public.edicion as E
WHERE C.nousuario <> 00000 and O.id = A.ofertaid and A.isbn = C.isbn
    and C.fecha <= O.fecha2 and C.fecha >= O.fecha1 and E.isbn = C.isbn

-- Compras que han hecho usuarios no fidelizados sin estar en oferta en ese momento (pero alguna vez sí)

SELECT C.isbn, C.nousuario, C.metodo, C.fecha, (E.precio * 0,9) as precio_fid_no_oferta
FROM public.compra as C, public.afectaoferta as A, public.oferta as O
WHERE C.nousuario <> 00000 and O.id = A.ofertaid and A.isbn = C.isbn
    and C.fecha > O.fecha2 and C.fecha < O.fecha1

-- Compras que han hecho usuarios no fidelizados de libros que nunca han estado en oferta

SELECT C.isbn, C.nousuario, C.metodo, C.fecha, (E.precio * 0,9) as precio_fid_nunca_oferta
FROM public.compra as C, public.afectaoferta as A, public.edicion as E
WHERE C.nousuario <> 00000 and O.id not in public.afectaoferta and E.isbn = C.isbn
