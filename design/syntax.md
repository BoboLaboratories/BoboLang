---
documentclass: report
numbersections: true
geometry: "a4paper, left=1.5cm, right=1.5cm, top=2cm, bottom=2cm"
header-includes: |
  \usepackage[]{xcolor}
  \definecolor{MyGreen}{RGB}{0, 128, 0}
  \newcommand{\cmd}[1]{\textcolor{MyGreen}{\{\, #1 \,\}} \;}
  \newcommand{\var}[1]{\langle #1 \rangle \;}
  \newcommand{\term}[1]{\texttt{#1} \;}
---

# BoboLang {.unnumbered .unlisted}

\setcounter{tocdepth}{5}
\tableofcontents

# Terminal symbols regex

\begin{center}
\begin{tabular}{ r c l }
$\term{ID}$         & $::=$ & $\Bigl(\term{a} + ... + \term{Z} + \bigl(\term{\_}(\term{\_})^*(\term{a} + ... + \term{Z} + \term{0} + ... + \term{9})\bigl)\Bigl)\Bigl(\term{a} + ... + \term{Z} + \term{0} + ... + \term{9} + \term{\_}\Bigl)^*$ \\\\
% --------------------------------------------------------------------------------------------------------------------%
\end{tabular}
\end{center}


# File

\begin{center}
\begin{tabular}{ r c l }
$\var{file}$        & $::=$ & $\var{importlist} \var{filep}$                        \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{importlist}$  & $::=$ & $\term{import ID} \var{importidp} \var{importlist}$   \\
                    &  $|$  & $\varepsilon$                                         \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{importidp}$   & $::=$ & $\term{.ID} \var{importidp}$                          \\
                    &  $|$  & $\varepsilon$                                         \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{filep}$       & $::=$ & $\term{module ID \{} \var{modulep} \term{\}}$         \\
                    &  $|$  & $\var{scriptp}$                                       \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{modulep}$     & $::=$ & $\var{assignment} \var{modulep}$                      \\ 
                    &  $|$  & $\var{fun} \var{modulep}$                             \\
                    &  $|$  & $\varepsilon$                                         \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{scriptp}$     & $::=$ & $\var{fun} \var{scriptp}$                             \\
                    &  $|$  & $\var{stat} \var{scriptp}$                            \\
                    &  $|$  & $\varepsilon$
\end{tabular}
\end{center}

# Functions

\begin{center}
\begin{tabular}{ r c l }
$\var{fun}$         & $::=$ & $\var{funmod} \term{fun} \term{ID (} \var{arglist} \term{) \{} \var{statlist} \term{\}}$ \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{funmod}$      & $::=$ & $\term{private}$                                             \\
                    &  $|$  & $\term{public}$                                              \\
                    &  $|$  & $\varepsilon$                                                \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{statlist}$    & $::=$ & $...$                                                        \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{arglist}$     & $::=$ & $\var{var} \var{arglistp}$                                   \\
                    &  $|$  & $\varepsilon$                                                \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{arglistp}$    & $::=$ & $\term{,} \var{var} \var{arglistp}$                          \\
                    &  $|$  & $\term{=} \var{expr} \var{defarglistp}$                      \\
                    &  $|$  & $\varepsilon$                                                \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{defarglistp}$ & $::=$ & $\term{,} \var{var} \term{=} \var{expr} \var{defarglistp}$   \\
                    &  $|$  & $\varepsilon$                                                \\\\
\end{tabular}
\end{center}


```text
    [public] fun start(const a, [var] b) {
        ...
    }
    
    fun ciao(x, y = 2) {
        ...
    }

```

# Statements

## Assignments 

\begin{center}
\begin{tabular}{ r c l }
$\var{var}$         & $::=$ & $\var{varmod} \term{ID}$                                     \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{varmod}$      & $::=$ & $\term{const}$                                               \\
                    &  $|$  & $\term{var}$                                                 \\
                    &  $|$  & $\varepsilon$                                                \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{assignment}$  & $::=$ & $\var{var} \var{assignmentp} \var{expr}$                     \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{assignmentp}$ & $::=$ & $\term{=} \var{var} \var{assignmentp} $                      \\
                    &  $|$  & $\term{=}$                                                   \\\\
\end{tabular}
\end{center}

## Consequence

\begin{center}
\begin{tabular}{ r c l }
$\var{conseq}$      & $::=$ & $\var{bexpr} \term{->} \var{statlist}$ \\\\
\end{tabular}
\end{center}


###
```text
    x <= 0 -> io.err("x must be positive")
```

###
```text
    (x <= 0) -> {
        io.err("x must be positive")
        return
    }

```

## Check

\begin{center}
\begin{tabular}{ r c l }
$\var{check}$       & $::=$ & $\term{check \{} \var{checklist} \term{\}}$ \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{checklist}$   & $::=$ & $\var{conseq} \var{checklistp}$ \\\\
% --------------------------------------------------------------------------------------------------------------------%
$\var{checklistp}$  & $::=$ & $\var{conseq} \var{checklistp}$ \\

                      &  $|$  & $\varepsilon$
\end{tabular}
\end{center}

### Simple
```text
    check {
        x > 0 -> io.print("nyum")
    }
```

### Check, else
```text
    check {
        is_number(x) -> io.print("nyum")
        true         -> return
    }
```

### Check, else-if, else
```text
    check {
        is_number(x) -> io.print("nyum")
        is_string(x) -> io.print("nope")
        true         -> return
    }
```

### Nested usage 
```text
    check {
        is_integer(x) -> check {
            x == 0 -> io.print("Zero")
            x < 0  -> io.print("Negative")
            x > 0  -> io.print("Positive")
        }
    
        is_string(x) -> ...
    }
```
