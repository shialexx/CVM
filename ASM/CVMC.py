from lark import Lark

def transform_to_c_bytearray(bytearray):
    c_bytearray = "{"
    for byte in bytearray:
        c_bytearray += f"0x{byte:02X}, "
    c_bytearray = c_bytearray.rstrip(", ") + "}"
    return c_bytearray

# Define the grammar for the assembly-like code
asm_grammar = """
    start: instruction+

    instruction: push | print | call | input | add | sub | mul | cmp | tag | jmp | je | jne | pop | comment | mov

    mov: "mov" NAME "," value
    push: "push" value
    value: NUMBER | STRING | NAME

    print: "print"
    input: "input"
    add: "add" NAME "," value
    sub: "sub" NAME "," value
    mul: "mul" NAME "," value
    cmp: "cmp" NAME "," value
    pop: "pop"
    tag: "tag" NAME
    je: "je" NAME
    jne: "jne" NAME
    jmp: "jmp" NAME
    comment: "//" 
    call: "call" NAME
    

    %import common.CNAME -> NAME
    %import common.NUMBER
    %import common.ESCAPED_STRING -> STRING
    %import common.WS
    %ignore WS
"""


# Create the parser using the given grammar and transformer
asm_parser = Lark(asm_grammar, parser='lalr')

# Test code to parse
asm_code = open("program.asm", 'r').read()

bytecode = b""

# Parse the code and get the output
asm_ast = asm_parser.parse(asm_code)

base = asm_ast.children

regs = {
    'ax': 1,
    'bx': 2,
    'cx': 3,
    'dx': 4,
    'ex': 5,
}

tags = {

}

for i in base:
    cmd = i.children[0]
    cmd_type = cmd.data
    match(cmd_type):
        case 'mov':
            r1 = cmd.children[0].value
            r2 = cmd.children[1].children[0].value
            r2_type = cmd.children[1].children[0].type
            if r2_type == "NAME":
                bytecode += b"\x0F" + regs[r1].to_bytes(1, 'little') + regs[r2].to_bytes(1, 'little')
            if r2_type == "NUMBER":
                bytecode += b"\x0E" + regs[r1].to_bytes(1, 'little') + int(r2).to_bytes(4, 'little')
        case 'push':

            var_type = cmd.children[0].children[0].type
            var_value = cmd.children[0].children[0].value
            if var_type == "STRING":
                var_value = var_value.replace('"', '')
                strlen = len(var_value)
                bytecode += b"\x02" + strlen.to_bytes(1, 'little') + var_value.encode()
            if var_type == "NUMBER":
                bytecode += b"\x01" + int(var_value).to_bytes(4, 'little')
            if var_type == "NAME":
                bytecode += b"\x03" + regs[var_value].to_bytes(1, 'little')

        case 'print':
            bytecode += b"\x04"
        case 'call':
            var_value = cmd.children[0].value
            strlen = len(var_value)
            bytecode += b"\x02" + strlen.to_bytes(1, 'little') + var_value.encode()
            bytecode += b"\x0D"
        case "input":
            bytecode += b"\x05"
        case "add":
            r1 = cmd.children[0].value
            r2 = cmd.children[1].children[0].value
            r2_type = cmd.children[1].children[0].type
            if r2_type == "NAME":              
                bytecode += b"\x06" + regs[r1].to_bytes(1, 'little') + regs[r2].to_bytes(1, 'little')

            if r2_type == "NUMBER":
                bytecode += b"\x0E" + regs['ex'].to_bytes(1, 'little') + int(r2).to_bytes(4, 'little') #mov ex, [number]
                bytecode += b"\x06" + regs[r1].to_bytes(1, 'little') + regs['ex'].to_bytes(1, 'little')
        case "sub":
            r1 = cmd.children[0].value
            r2 = cmd.children[1].children[0].value
            r2_type = cmd.children[1].children[0].type
            if r2_type == "NAME":              
                bytecode += b"\x07" + regs[r1].to_bytes(1, 'little') + regs[r2].to_bytes(1, 'little')

            if r2_type == "NUMBER":
                bytecode += b"\x0E" + regs['ex'].to_bytes(1, 'little') + int(r2).to_bytes(4, 'little') #mov ex, [number]
                bytecode += b"\x07" + regs[r1].to_bytes(1, 'little') + regs['ex'].to_bytes(1, 'little')
        case "mul":
            r1 = cmd.children[0].value
            r2 = cmd.children[1].children[0].value
            r2_type = cmd.children[1].children[0].type
            if r2_type == "NAME":              
                bytecode += b"\x08" + regs[r1].to_bytes(1, 'little') + regs[r2].to_bytes(1, 'little')

            if r2_type == "NUMBER":
                bytecode += b"\x0E" + regs['ex'].to_bytes(1, 'little') + int(r2).to_bytes(4, 'little') #mov ex, [number]
                bytecode += b"\x08" + regs[r1].to_bytes(1, 'little') + regs['ex'].to_bytes(1, 'little')

        case "cmp":
            r1 = cmd.children[0].value
            r2 = cmd.children[1].children[0].value
            r2_type = cmd.children[1].children[0].type
            if r2_type == "NAME":              
                bytecode += b"\x09" + regs[r1].to_bytes(1, 'little') + regs[r2].to_bytes(1, 'little')

            if r2_type == "NUMBER":
                bytecode += b"\x0E" + regs['ex'].to_bytes(1, 'little') + int(r2).to_bytes(4, 'little') #mov ex, [number]
                bytecode += b"\x09" + regs[r1].to_bytes(1, 'little') + regs['ex'].to_bytes(1, 'little')

        case "pop":
            bytecode += b"\x10"
        case "tag":
            var_value = cmd.children[0].value
            tags[var_value] = len(bytecode)
        case "jmp":
            var_value = cmd.children[0].value
            bytecode += b"\x0C" + tags[var_value].to_bytes(1, 'little')
        case "je":
            var_value = cmd.children[0].value
            bytecode += b"\x0A" + tags[var_value].to_bytes(1, 'little')
        case "jne":
            var_value = cmd.children[0].value
            bytecode += b"\x0B" + tags[var_value].to_bytes(1, 'little')
    

print(transform_to_c_bytearray(bytecode) + ";")