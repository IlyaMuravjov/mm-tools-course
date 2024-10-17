# Task

- **Course:** https://hwproj.ru/courses/20022
- **Task:** 1.1. IO_Flush: experiment

# Setup

- **OS:** Ubuntu 22.04 LTS
- **Compiler:** GCC 11.4.0
- **Script:** [run_experiments.sh](run_experiments.sh)

# Result

 <!-- Have to use html tables to put code block with empty last line inside table -->

<table>
<tr>
<th> Added <code>\n</code> </th>
<th> Removed <code>fflush</code> </th>
<th> Redirected to File </th>
<th> Output </th>
</tr>

<tr>
<td> false </td>
<td> false </td>
<td> false </td>
<td>

```plaintext
STDOUT
STDERR
```

</td>
</tr>

<tr>
<td> false </td>
<td> false </td>
<td> true </td>
<td>

```plaintext
STDOUT
STDERR
```

</td>
</tr>

<tr>
<td> false </td>
<td> true </td>
<td> false </td>
<td>

```plaintext
STDOUT
STDERR
```

</td>
</tr>

<tr>
<td> false </td>
<td> true </td>
<td> true </td>
<td>

```plaintext
STDERRSTDOUT

```

</td>
</tr>

<tr>
<td> true </td>
<td> false </td>
<td> false </td>
<td>

```plaintext
STDOUT

STDERR

```

</td>
</tr>

<tr>
<td> true </td>
<td> false </td>
<td> true </td>
<td>

```plaintext
STDOUT

STDERR

```

</td>
</tr>

<tr>
<td> true </td>
<td> true </td>
<td> false </td>
<td>

```plaintext
STDOUT

STDERR

```

</td>
</tr>

<tr>
<td> true </td>
<td> true </td>
<td> true </td>
<td>

```plaintext
STDERR
STDOUT

```

</td>
</tr>

</table>

# Conclusion

- Removing `fflush` can cause `STDERR` to appear before `STDOUT` (it seems to only happen when redirecting to file)
- Adding `\n` surprisingly adds a new line
