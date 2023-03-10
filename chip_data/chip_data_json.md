# Chip Data File JSON Format

## General Requirements/Guidelines

- All file names must contain the `.json` extension. Otherwise, there are no
  additional file name requirements.

- The file format is flexible so that a chip can be described by one file or
  many files. It is up to the data owner to determine how the data is split into
  multiple files, if desired. The only requirement is that all data contained in
  a file must be the same for all chip model/EC levels described by the file
  (see the `model_ec` property). Otherwise, the necessary data must be split
  into multiple files.

- All files for a chip model/EC level must be contained within the same
  directory (no sub-directories). A directory could contain files for more than
  one chip model/EC level. A directory can contain non-JSON files, but all JSON
  files in the directory must be Chip Data JSON.

---

## 1) Base JSON Object

All files will have the same format, which is a JSON object containing the
properties defined below.

### 1.1) Property: `version` (required, integer)

A positive integer representing the Chip Data file format version. This will be
useful later if the file format needs to change. The initial version is `1` and
is currently the only supported version.

### 1.2) Property: `model_ec` (required, array of strings)

An array of strings representing the chip models and EC levels represented by
this file. See appendix for supported values.

**Important:** This property allows us to reuse the same data definition for
multiple chip models and/or EC levels. It is intentionally limited to the file
scope for simplicity, maintainability, and readability. Separate files will be
required if any data differs between chip models and/or EC levels.

### 1.3) Property: `registers` (optional, object)

A JSON object where the keys are register names and the values are Register JSON
objects.

A register name must be alphanumeric and/or underscores. No spaces or other
symbols allowed. The register names will be displayed in log files.

This property will be combined with all other definitions of this property in
all other Chip Data files for the target chip models and EC levels defined in
the `model_ec` property.

Not all Chip Data files will need to define this property, which is why the
field is optional. However, after combining all the files, at minimum, an entry
should exist for every register referenced in the Isolation Node JSON objects.

### 1.4) Property: `isolation_nodes` (optional, object)

A JSON object where the keys are isolation node names and the values are
Isolation Node JSON objects.

An isolation node name must be alphanumeric and/or underscores. No spaces or
other symbols allowed. The isolation node names will be displayed in log files.
Note that these typically matches the name of the register(s) targeted for
isolation, but it is not required.

This property will be combined with all other definitions of this property in
all other Chip Data files for the target chip models and EC levels defined in
the `model_ec` property.

Not all Chip Data files will need to define this property, which is why the
field is optional. However, after combining all the files there should be at
least one Isolation Node JSON object defined per attention type.

### 1.5) Property: `root_nodes` (optional, object)

A JSON object where the keys are attention types and the values are Root Node
JSON objects. See appendix for supported attention types.

This property will be combined with all other definitions of this property in
all other Chip Data files for the target chip models and EC levels defined in
the `model_ec` property.

Not all Chip Data files will need to define this property, which is why the
field is optional. However, after combining all the files there should be a Root
Node JSON object defined for all attentions types supported on the target chip
models and EC levels.

### 1.6) Property: `capture_groups` (optional, object)

A JSON object where the keys are capture group names and the values are an array
of Capture Register JSON objects. The array is used to maintain the order in
which registers are captured and stored in the FFDC.

This property will be combined with all other definitions of this property in
all other Chip Data files for the target chip models and EC levels defined in
the `model_ec` property.

Not all Chip Data files will need to define this property, which is why the
field is optional.

**Important:** All registers referenced by the Isolation Node Rules should be
automatically captured for FFDC. Therefore, the capture groups should only be
defined if additional registers are needed.

---

## 2) Register JSON object

This describes a hardware register that may be used for isolation or simply
captured for FFDC. At a minimum, a Register JSON object must exist for each
register referenced by the Isolation Node JSON objects.

### 2.2) Property: `reg_type` (optional, string)

A string representing the register type, which specifies the register size, bit
order, etc. Most registers are standard SCOM registers. So the default value of
`SCOM` will be used when this property is not specified. See appendix for
supported types.

### 2.3) Property: `access` (optional, string)

The hardware operation accessibility. Most registers have read and write access.
So the default value of `RW` will be used when this property is not specified.
See appendix for supported types.

### 2.4) Property: `instances` (required, object)

It is possible that a register could have multiple instances within a chip. For
example, the same register could exist for each core on a processor chip.
Generally, the isolation rules and bit definition for registers like these are
the same for each instance. The only difference would be the register addresses
associated with each instance. So, instead of repeating the same information
multiple times, this object will contain the addresses for each unique instance
of the register.

The key is a register instance, which a unique integer value for the logical
instance of the register. Note that the default value of `0` should be used for
any single-instance registers.

The value is a string representation of the address for this register instance.
The length of this hexadecimal integer is dependent on the `reg_type` property.
Note that value is intentionally a string for readability of the file. See
appendix for number format requirements.

---

## 3) Isolation Node JSON object

This object is a node in an attention isolation tree. It will contain:

- A set of rules describing the registers and bit operations required to
  determine if there are active attentions for supported attention types.

- A bit definition of the rule results indicating whether an active attention
  has been found or if it originated from another node in the isolation tree.

- A list of registers associated with this isolation node to store in logs for
  additional debug, if necessary.

**Important:** A node typically represents a FIR. However, other hardware
registers, or a combination of registers, could be used as well.

### 3.1) Property: `reg_type` (optional, string)

All registers used by this node must be of the same type. This dispels any
ambiguity that may occur with the bitwise operations defined by the `rules`
property. It also defines the maximum number of entries that can be defined in
the `bits` property and the order of the bit positions (left to right vs. right
to left).

Most registers are standard SCOM registers. So the default value of `SCOM` will
be used when this property is not specified. See appendix for supported types.

### 3.2) Property: `instances` (required, array of integers)

An array of integers representing the instances defined for this isolation node.

### 3.3) Property: `rules` (required, array of objects)

An array of Isolation Rule JSON objects.

### 3.4) Property: `bits` (required, object)

The keys are bit positions of the resulting rule output, which can be integers
or integer ranges. See appendix for the number range format.

The values are Isolation Bit JSON objects.

### 3.5) Property: `capture_groups` (optional, array of objects)

An array of Capture Group JSON objects. When isolating to this node, these
objects provide a list of registers that should be captured and stored in log
files for additional debug, if necessary. The array is used to maintain the
order in which registers are captured and stored in the FFDC.

---

## 4) Isolation Rule JSON Object

A rule specifies how an attention is raised from a register. A rule is
constructed by a series of expressions (see `expr` below) which are bitwise
operations on a register or set of registers. Any active bits (i.e. bit is set
to 1) in the result of the expressions will indicate the active attentions for
the rule.

### 4.1) Property: `attn_type` (required, array of strings)

An array of strings representing the attention types targeted for this rule.
Some rules can be defined for more than one attention type, which is why this is
an array. See appendix for supported values.

### 4.2) Property: `node_inst` (required, array of integers)

An array of integers representing the isolation node instances targeted for this
rule. Some rules can be defined for more than one isolation node instance, which
is why this is an array.

### 4.3) Property: `expr` (required, object)

An Isolation Rule Expression JSON object.

---

## 5) Isolation Rule Expression JSON object

Expressions are used to characterize bitwise operations carried out against
registers and/or integer constants. For example, `~some_register & 0xffff` will
take the contents of `some_register`, apply a bitwise NOT operation, and then
AND that value with the integer `0xffff`. This example will generate the
following Isolation Rule JSON object:

```json
{
  "attn_type": ["CS", "RE", "SPA"],
  "node_inst": [0, 1],
  "expr": {
    "expr_type": "and",
    "exprs": [
      {
        "expr_type": "not",
        "expr": {
          "expr_type": "reg",
          "reg_name": "some_register",
          "reg_inst": { "0": 2, "1": 4 }
        }
      },
      {
        "expr_type": "int",
        "int_value": "0xffff"
      }
    ]
  }
}
```

For simplicity, the register and integer sizes will be defined by the `reg_type`
property of the Isolation Node JSON object. This ensures all values are the same
length, eliminating the ambiguity caused by variable register and integer sizes.

### 5.1) Property: `expr_type` (required, string)

This property is required for Isolation Rule Expression JSON objects. All other
properties depend on this property's value.

#### 5.1.1) Expression Type Value: `reg`

This is a special expression that indicates the value of the target register
should be used in this expression. Generally, this means reading the register
value from hardware.

##### 5.1.1.1) Additional Property: `reg_name` (required, string)

The name of the target register. This would be the same value as the keys used
in the `registers` property of the Base JSON object.

##### 5.1.1.2) Additional Property: `reg_inst` (optional, object)

An instance map from node instance (key) to register instance (value). See
appendix for details on instance maps. If this property is not specified, the
register instance value will always match the node instance value.

**Important:** All nodes instances in the `node_inst` property of the containing
Isolation Rule JSON object must exist as keys in this instance map.

#### 5.1.2) Expression Type Value: `int`

This simply represents an integer constant.

##### 5.1.2.1) Additional Property: `int_value` (required, string)

A right-justified, unsigned integer constant value. This number will be
represented as a string for readability of hexadecimal numbers. See appendix for
the number format rules.

**Important:** The size of the constant is determined by the register type
specified in the `reg_type` property of the containing Isolation Node JSON
object. So if the register type is `SCOM`, which is 64-bit, and the value is
`0xff`, the actual number will be `0x00000000000000ff`.

#### 5.1.3) Expression Type Values: `and` and `or`

Bitwise AND and OR operations (i.e. `EXPR_1 & EXPR_2 | EXPR_3`).

##### 5.1.3.1) Additional Property: `exprs` (required, array of objects)

An array of Isolation Rule Expression JSON objects. There must be at least two
entries in the array, but more can be specified. Regardless of the array size,
the same operation will be applied to all entries of the array after each
sub-expression has been resolved.

#### 5.1.4) Expression Type Value: `not`

A bitwise NOT operation (i.e. `~EXPR`).

##### 5.1.4.1) Additional Property: `expr` (required, object)

An Isolation Rule Expression JSON object. The NOT operation will be applied
after the sub-expression has been resolved.

#### 5.1.5) Expression Type Values: `lshift` and `rshift`

Left and right shift operations (i.e. `(EXPR << 8) >> 16`).

##### 5.1.5.1) Additional Property: `expr` (required, object)

An Isolation Rule Expression JSON object. The shift operation will be applied
after the sub-expression has been resolved.

##### 5.1.5.2) Additional Property: `shift_value` (required, integer)

The shift value. Must be a positive integer.

**Important:** Any values shifted beyond the defined register length will be
lost. For example, given the register length of 2 bytes, the expression
`(0xffff << 8) >> 8` will resolve to `0x00ff`.

---

## 6) Isolation Bit JSON Object

These objects provide the metadata for each bit defined by the isolation rules.

### 6.1) Property: `desc` (required, string)

A human-readable description of this bit, which will be displayed in log files.
It is highly recommended to keep this description short and concise (~50
characters) because longer descriptions will likely be truncated depending on
the application.

### 6.2) Property: `child_node` (optional, object)

When specified, this property indicates the attention originated from another
isolation node. This property is a simple JSON object with the following
properties:

#### 6.2.1) Subproperty: `name` (required, string)

The name of the target isolation node. This would be the same value as the keys
used in the `isolation_nodes` property of the Base JSON object.

#### 6.2.2) Subproperty: `inst` (optional, object)

An instance map from parent node instance (key) to child node instance (value).
See appendix for details on instance maps. If this property is not specified,
the child node instance value will always match the parent node instance value.

**Important:** All nodes instances in the `instances` property of the containing
Isolation Node JSON object must exist as keys in this instance map.

### 6.3) Property: `capture_groups` (optional, array of objects)

An array of Capture Group JSON objects. When isolating to this bit, these
objects provide a list of registers that should be captured and stored in log
files for additional debug, if necessary. The array is used to maintain the
order in which registers are captured and stored in the FFDC.

---

## 7) Capture Group JSON Object

This describes a capture group and which instances needs to be captured for
FFDC.

### 7.1) Property: `group_name` (required, string)

A capture group name. This value is found in the keys of the `capture_groups`
property of the Base JSON object.

### 7.2) Property: `group_inst` (required, object)

This is an instance map from node instance (key) to group instance (value). See
appendix for details on instance maps.

**Important:** If a node instance is not defined in this map, the group simply
will not be captured.

---

## 8) Root Node JSON object

The attention isolation tree can be traversed via the Isolation Node JSON
objects. A Root Node JSON object is simply the root of the isolation tree for a
specific attention type. As a reminder, the attention types are the keys of the
`root_nodes` property of the Base JSON object.

### 8.1) Property: `name` (required, string)

The string name of the isolation node that will be the root of this isolation
tree. This value is found in the keys of the `isolation_nodes` property of the
Base JSON object.

### 8.2) Property: `inst` (required, integer)

The logical instance of the root node. This value is found in the `node_inst`
property of an Isolation Node Rule object associated with the target isolation
node.

---

## 9) Capture Register JSON Object

This describes a register and which instances needs to be captured for FFDC.

### 9.1) Property: `reg_name` (required, string)

A register name. This value is found in the keys of the `registers` property of
the Base JSON object.

### 9.2) Property: `reg_inst` (required, object)

This is an instance map from group instance (key) to register instance (value).
See appendix for details on instance maps.

**Important:** If a group instance is not defined in this map, the register
simply will not be captured.

---

## 10) Appendix

### 10.1) Number Formats

All numbers in the Chip Data files are unsigned integers. When possible, the
JSON integer data type should be used for performance and file size.
Unfortunately, the JSON integer data type only allows base 10 numbers. For
readability, some properties allow a string representation of the number
specifically for numbers that are better served to be in the hexadecimal format.
In which case, the string value must start with '0x'.

### 10.2) Number Ranges

A number range represents consecutive ascending or descending numbers (including
both endpoints) and are expressed using a colon (e.g. "8:15" or "15:8").

### 10.3) Instance Maps

Instance Maps are a simple JSON object mapping one type of instance to another.
Say for example you are targeting a specific capture group instance and you want
to capture all of the associated registers for FFDC. The Capture Register JSON
object instance maps define how group instances map to register instances.

### 10.4) Supported Chip Models and EC Levels

| Value         | Description            |
| ------------- | ---------------------- |
| `EXPLORER_11` | Explorer chip EC 1.1   |
| `EXPLORER_20` | Explorer chip EC 2.0   |
| `ODYSSEY_10`  | Odyssey chip EC 1.0    |
| `P11S_10`     | P11 Spinal chip EC 1.0 |
| `P11T_10`     | P11 Tap chip EC 1.0    |

### 10.5) Supported Attention Types

**Important:** A chip checkstop is often associated with a system checkstop,
which is certainly true for processor chips. However, systems may, but not
always, recover from a chip checkstop on a memory buffer or other peripheral
chips.

| Value | Description                                                        |
| ----- | ------------------------------------------------------------------ |
| `CS`  | Chip checkstop hardware attention.                                 |
| `UCS` | Local checkstop hardware attention within a chip unit.             |
| `RE`  | Recoverable hardware attention.                                    |
| `SPA` | SW or HW event requiring action by the service processor firmware. |

### 10.6) Supported Register Types

- POWER Systems SCOM register (default)

  - Property value: `SCOM`
  - Address length: 4 bytes
  - Register length: 8 bytes
  - Bit order: ascending (0-63, left to right)

- POWER Systems Indirect SCOM register
  - Property value: `IDSCOM`
  - Address length: 8 bytes
  - Register length: 8 bytes
  - Bit order: ascending (0-63, left to right)

### 10.7) Supported Register Access Types

| Value | Description                     |
| ----- | ------------------------------- |
| `RO`  | read-only access                |
| `WO`  | write-only access               |
| `RW`  | read and write access (default) |
