---
applyTo: '**/*.h,**/*.hpp'
---

## Goal

When I ask for comments for any **function, method, class, struct, enum, file, or module**, always generate **Doxygen-compatible** comments.

Use **standard Doxygen tags** and clear, concise descriptions. Assume the codebase uses Doxygen for documentation generation.

---

## General Rules

1. **Always use Doxygen format**
   - Use the `/*! ... */` block style

2. **Language-Aware**
   - For **C / C++ / C++17+**: use standard Doxygen syntax

3. **Be explicit**:
   - Document **what** the entity does
   - Document **how** to use it if not obvious
   - Document **preconditions / postconditions / side effects** when relevant
   - Avoid restating the identifier’s name as the only description

4. **Be consistent and complete**:
   - Every template parameter → `\tparam {template parameter type} {template parameter description}` line
   - Every parameter → `\param[] {parameter name} {parameter description}` line
     - Pointers or output parameters → document if they are input, output, or in/out (`\param[in]`, `\param[out]`, `\param[in,out]`)
   - Non-void returns → `\retval {data type} {meaning of the result}` line
   - Possible errors/exceptions → `\throws` or `\exception`

5. **Write in third person, present tense**
   - Example: “Computes the hash of the input buffer” (not “Compute” or “This function computes…” unless it improves clarity)

---

## Doxygen Tag Usage

When generating comments, prefer the following tags:

### For Files / Modules

Place at the top of source/header files:

```cpp
/*! \file MyFileName.h
    \brief Short description of the file's purpose.
    \date The date in MM/DD/YYYY format.
    \version The current version of the file.
    \since The version or date since this file has been present.
    \author The author of the file.
 */
```

#### Other considerations for files/modules:

- Always include '\file', '\brief', '\date', '\version', '\since', and '\author' tags
- Use `\ingroup` for logical modules or groups
- Use `\copyright` for licensing info if needed

### For Functions / Methods

```cpp
/*! \brief Short, one-line summary in sentence form.
    \details Optional longer description that explains behavior, algorithms,
    constraints, and important usage notes.
    \pre Preconditions that must hold before calling.
    \post Postconditions guaranteed after successful return.
    \warning Important warnings about misuse or edge cases.
    \tparam T Description of the template parameter T.
    \param[in]  paramName  Description of the parameter (role, units, valid range).
    \param[out] result     Description of output parameter if any.
    \param[in,out] state   Description of in/out parameter behavior.
    \note Additional notes or remarks.
    \retval T Description of the return value. If the function is `void`, omit \return.
    \throws SomeExceptionType Description of condition(s) that cause this.
    \throws AnotherException  Optional additional thrown types.
    \note Additional notes or remarks.
    \see RelatedFunction(), RelatedClass
    \date The date in MM/DD/YYYY format.
    \version The current version of the file.
    \since The version or date since this file has been present.
    \author The author of the file.
 */
```

#### Other considerations for Files / Modules:

- Always include '\brief', '\date', '\version', '\since', and '\author' tags
- Include `\param` for every parameter in the function signature
  - If direction is clear (like const references or pointers), tag as @param[in], @param[out], or @param[in,out].
- Include `\retval` for non-void functions, even if obvious.
- If exceptions are thrown, use `\throws` / `\exception`.
- Use `\overload` for overloaded functions to link them
- Use `\deprecated` for deprecated functions with guidance on alternatives

### For Classes / Structs / Enums

#### For Classes

```cpp
/*! \class ClassName HeaderFile.h Path/To/HeaderFile.h
    \brief Short, one-line summary in sentence form.
    \details Optional longer description that explains behavior, design decisions,
    constraints, and important usage notes.
    \warning Important warnings about misuse or edge cases.
    \tparam T Description of the template parameter T.
    \note Additional notes or remarks.
    \date The date in MM/DD/YYYY format.
    \version The current version of the file.
    \since The version or date since this file has been present.
    \author The author of the file.
 */
```

#### For Structs

```cpp
/*! \struct StructName HeaderFile.h Path/To/HeaderFile.h
    \brief Short, one-line summary in sentence form.
    \details Optional longer description that explains behavior, design decisions,
    constraints, and important usage notes.
    \warning Important warnings about misuse or edge cases.
    \tparam T Description of the template parameter T.
    \note Additional notes or remarks.
    \date The date in MM/DD/YYYY format.
    \version The current version of the file.
    \since The version or date since this file has been present.
    \author The author of the file.
 */
```

#### For Enums

```cpp
/*! \enum EnumName
    \brief Short, one-line summary in sentence form.
    \details Optional longer description that explains behavior, design decisions,
    constraints, and important usage notes.
    \warning Important warnings about misuse or edge cases.
    \tparam T Description of the template parameter T.
    \note Additional notes or remarks.
    \date The date in MM/DD/YYYY format.
    \version The current version of the file.
    \since The version or date since this file has been present.
    \author The author of the file.
 */
```

#### Other considerations for Classes / Structs / Enums:

- Always include '\brief', '\date', '\version', '\since', and '\author' tags
- If the type is a POD container, data carrier, or configuration struct, explicitly state that purpose in the `\brief`.
- For the member variables of each class, struct, and enum (whether private, public, or protected), use the format below to document the member variables.

### For Member Variables

```cpp
/*! \var memberVariable
    \brief Short, one-line summary in sentence form.
    \details Optional longer description that explains behavior, constraints,
    and important usage notes.
 */
```

### Style Guidelines

1.  **Clarity over brevity**
    - Prefer: “Computes the SHA-256 digest of the given buffer.”
    - Avoid: “Computes hash.”
2.  **Avoid redundancy**
    - Don’t just repeat the parameter name.
    - Bad: `\param` size The size.
    - Good: `\param` size Number of bytes to read from the buffer.

3.  **Mention units & ranges**
    - E.g.: `\param` timeoutMs Timeout in milliseconds. Must be >= 0.

4.  **Error behavior**
    - If the function can fail or return error codes, describe how.
    - Note: “Returns -1 on failure; check errno for details.”

5.  **Thread-safety & ownership (if relevant)**
    - Mention whether functions/classes are thread-safe, reentrant, or own resources.

### How to Apply These Instructions

When I ask you to:

- “Add comments/documentation to this function/class/struct”
- “Generate Doxygen comments for this code”
- “Document this header in Doxygen format”

**You must:**

- **Inspect the signature and context** (parameters, return type, template params).
- **Generate a Doxygen comment block** immediately above the entity.
- Use the appropriate tags: `\file`, `\brief`, `\param`, `\return`, `\tparam`, `\throws`, `\retval`, `\note`, etc.
- Ensure the comments are correct, specific, and technically meaningful, not generic placeholders.
- If information is ambiguous or missing, make reasonable assumptions but keep them realistic and conservative.
