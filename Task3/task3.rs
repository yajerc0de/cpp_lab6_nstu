use std::io::{self, Write};

const M: f64 = 0.89;
const N: f64 = 0.08;
const P: f64 = -1.21;
const EPS: f64 = 1e-3;
const SIZE: usize = 4;

// Вывод вектора
fn print_vector(vec: &[f64], name: &str) {
    print!("{}: [", name);
    for (i, val) in vec.iter().enumerate() {
        print!("{:9.6}", val);
        if i < vec.len() - 1 {
            print!(", ");
        }
    }
    println!(" ]");
}

// Прямой метод – LU-разложение
fn solve_lu(a: &Vec<Vec<f64>>, b: &Vec<f64>) {
    let mut l = vec![vec![0.0; SIZE]; SIZE];
    let mut u = vec![vec![0.0; SIZE]; SIZE];

    for i in 0..SIZE {
        l[i][i] = 1.0;
    }

    // Разложение
    for i in 0..SIZE {
        for k in i..SIZE {
            let mut sum = 0.0;
            for j in 0..i {
                sum += l[i][j] * u[j][k];
            }
            u[i][k] = a[i][k] - sum;
        }
        for k in i + 1..SIZE {
            let mut sum = 0.0;
            for j in 0..i {
                sum += l[k][j] * u[j][i];
            }
            l[k][i] = (a[k][i] - sum) / u[i][i];
        }
    }

    println!("\n=== ПРЯМОЙ МЕТОД (LU-разложение) ===");

    // Решение Ly = b
    let mut y = vec![0.0; SIZE];
    for i in 0..SIZE {
        let mut sum = 0.0;
        for j in 0..i {
            sum += l[i][j] * y[j];
        }
        y[i] = b[i] - sum;
    }
    print_vector(&y, "Результат прямого хода (y)");

    // Решение Ux = y
    let mut x = vec![0.0; SIZE];
    for i in (0..SIZE).rev() {
        let mut sum = 0.0;
        for j in i + 1..SIZE {
            sum += u[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / u[i][i];
    }
    print_vector(&x, "Результат обратного хода (точные корни x)");
}

// Приближённый метод – Зейделя
fn solve_zeidel(a: &Vec<Vec<f64>>, b: &Vec<f64>) {
    println!("\n=== ПРИБЛИЖЁННЫЙ МЕТОД (Зейдель) ===");

    // Преобразование к виду x = Cx + f
    let mut c = vec![vec![0.0; SIZE]; SIZE];
    let mut f = vec![0.0; SIZE];

    for i in 0..SIZE {
        let diag = a[i][i];
        f[i] = b[i] / diag;
        for j in 0..SIZE {
            if i == j {
                c[i][j] = 0.0;
            } else {
                c[i][j] = -a[i][j] / diag;
            }
        }
    }

    // Проверка достаточного условия сходимости (норма C)
    let mut norm_c = 0.0;
    for i in 0..SIZE {
        let row_sum: f64 = c[i].iter().map(|&x| x.abs()).sum();
        if row_sum > norm_c {
            norm_c = row_sum;
        }
    }
    println!("Норма матрицы C (бесконечная): {:.6}", norm_c);
    if norm_c >= 1.0 {
        println!("Внимание: достаточное условие сходимости не выполнено (||C|| >= 1).");
        println!("Итерации могут расходиться.");
    }

    // Начальное приближение x0 = f
    let mut x_curr = f.clone();
    let mut x_next = vec![0.0; SIZE];

    // Шапка таблицы
    println!("\n-------------------------------------------------------------------------------");
    println!("|   N   |    x1     |    x2     |    x3     |    x4     |       eps_n        |");
    println!("-------------------------------------------------------------------------------");

    // Нулевая итерация
    print!("| {:5} | ", 0);
    for &val in &x_curr {
        print!("{:9.6} | ", val);
    }
    println!("{:18} |", "—");

    let mut step = 0;
    let mut eps_n = 1.0;

    while eps_n > EPS && step < 200 {
        step += 1;
        x_next = x_curr.clone();

        // Одна итерация Зейделя
        for i in 0..SIZE {
            let mut sum = f[i];
            for j in 0..SIZE {
                sum += c[i][j] * x_next[j];
            }
            x_next[i] = sum;
        }

        // Оценка погрешности (максимальная разность между соседними итерациями)
        eps_n = 0.0;
        for i in 0..SIZE {
            let diff = (x_next[i] - x_curr[i]).abs();
            if diff > eps_n {
                eps_n = diff;
            }
        }

        x_curr = x_next.clone();

        print!("| {:5} | ", step);
        for &val in &x_curr {
            print!("{:9.6} | ", val);
        }
        println!("{:18.6e} |", eps_n);
    }
    println!("-------------------------------------------------------------------------------");

    println!("\nПриближённые корни с точностью {}:", EPS);
    print_vector(&x_curr, "x*");
}

fn main() {
    // Матрица A для варианта 11
    let a = vec![
        vec![M, -0.04, 0.21, -1.16],
        vec![0.25, -1.23, N, -0.09],
        vec![-0.21, N, 0.8, -0.13],
        vec![0.15, -1.31, 0.06, P],
    ];
    let b = vec![-1.24, P, 2.56, M];

    println!("Решение СЛАУ для варианта 11:");
    println!("M = {}, N = {}, P = {}\n", M, N, P);

    solve_lu(&a, &b);
    solve_zeidel(&a, &b);
}