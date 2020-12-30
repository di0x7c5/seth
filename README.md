# seth - control Siglent DSO throught the ethernet

This simple console program use linux sockets to communicate with DSO throught the ethernet and allowing to set all measurements parameters via SCPI messages.

All data read from DSO are printed on stdout, so it's straght forward ability to use it in stream processing using pipelines.

There is only one mandatory parameter needed to work and it's the IP address of the device You want to connect.

The program does not require external libraries to run and is fully independent.

## Custom SCPI

Program distinguishes two types of messages sent to DSO: command and query. The only difference between them is that the command only sends data to DSO and not expecting response, where query always awaits for DSO response after sending it.

To send custom SCPI commands use `-x` param. To force send a query use `-q` param.

For example to set 1V per div for channel 1 use command:

```
$ seth -s 192.168.2.2 -x "C1:VDIV 1V"
```

Program will automatically recognize the query type when the custom command will contain `?` char. Below command is treat as query and will return current value of voltage per div:

```
$ seth -s 192.168.2.2 -x "C1:VDIV?"
C1:VDIV 1.00E-01V
```

There are existing queries which don't contain `?` char in it. To force program to read response from DSO, use `-q` param:

```
$ seth -s 192.168.2.2 -q "SCDP"
<data>
```

## Examples

Get device ID:

```
$ seth -s 192.168.2.2 -x "*IDN?"
```

Reset DSO state:

```
$ seth -s 192.168.2.2 -x "*RST"
```

Get screenshot from DSO and save it as bitmap file:

```
$ seth -s 192.168.2.2 -q "SCDP" > screenshot.bmp
```

## SCPI Documentation

Document containing complete list of available SCPI commands for Siglent DSO is called __Programming Guide__ and is available here: https://int.siglent.com/u_file/document/ProgrammingGuide_PG01-E02C.pdf.
