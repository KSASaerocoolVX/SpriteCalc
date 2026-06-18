module;

#include <iostream>
#include <string_view>

export module app.help;

export namespace app {

void printHelp(std::ostream& output, std::string_view topic) {
    if (topic.empty()) {
        output << "help topics: basics, functions, matrix, polynomial, number, workspace, ui, bench\n";
        output << "examples: help matrix, help number, help bench\n";
        return;
    }

    if (topic == "basics") {
        output << "basics:\n";
        output << "  arithmetic: +, -, *, /, parentheses, unary minus\n";
        output << "  assignment: x = 10\n";
        output << "  exact division: 1 / 3 + 1 / 6 -> 1/2\n";
        output << "  exact decimals: 0.1 + 0.2 -> 3/10\n";
        output << "  complex: sqrt(-1) -> i, i * i -> -1\n";
        return;
    }

    if (topic == "functions") {
        output << "functions:\n";
        output << "  unary: phi, sqrt, sin, cos, tan, log, exp, abs\n";
        output << "  multi-arg: pow(a,b), gcd(a,b), eval(poly,x), integral(poly,a,b)\n";
        output << "  solving: solve(poly) for degree 1 and 2\n";
        output << "  matrix: transpose, trace, det, rank, identity, inverse\n";
        return;
    }

    if (topic == "matrix") {
        output << "matrix:\n";
        output << "  literal: [1,2;3,4]\n";
        output << "  exact rational entries: [1,2;3,4] / 2 -> [1/2,1;3/2,2]\n";
        output << "  latex: \\begin{bmatrix}1 & 2 \\\\ 3 & 4\\end{bmatrix}\n";
        output << "  ops: +, -, *, scalar ops\n";
        output << "  funcs: transpose(A), trace(A), det(A), rank(A), identity(n), inverse(A)\n";
        return;
    }

    if (topic == "polynomial") {
        output << "polynomial:\n";
        output << "  literal: {1,2,3} means 1 + 2x + 3x^2\n";
        output << "  latex-like: 1 + 2x + 3x^{2}\n";
        output << "  funcs: derivative(p), integral(p), integral(p, c), integral(p, a, b), solve(p), degree(p), eval(p, x)\n";
        return;
    }

    if (topic == "number") {
        output << "number theory:\n";
        output << "  phi(n), gcd(a,b)\n";
        output << "  modular: mod, modadd, modsub, modmul, modpow, modinv\n";
        output << "  residues: legendre(a,p), primroot(p)\n";
        output << "  congruences: linsolve(a,b,m), crt(a1,m1,a2,m2,...)\n";
        output << "  polynomial roots: rootsmod(poly,m), liftroots(poly,p,k)\n";
        return;
    }

    if (topic == "workspace") {
        output << "workspace:\n";
        output << "  vars        show variables\n";
        output << "  clear x     remove one variable\n";
        output << "  clear       remove all variables\n";
        return;
    }

    if (topic == "ui") {
        output << "ui:\n";
        output << "  theme off|minimal|cyber|preview\n";
        output << "  color auto|on|off\n";
        output << "  selftest\n";
        output << "  bench matrix\n";
        return;
    }

    if (topic == "bench") {
        output << "bench:\n";
        output << "  bench matrix  compare sequential and thread-pool matrix multiplication\n";
        return;
    }

    output << "unknown help topic: " << topic << '\n';
}

}
