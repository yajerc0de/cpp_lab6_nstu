use std::collections::VecDeque;
use std::io::{self, Write};

// Режим работы
#[derive(Debug, PartialEq)]
enum Choice {
    Demo = 1,
    Gen,
}

// Индекс бита для клетки (r, c)
fn get_bit_index(r: usize, c: usize, cols: usize) -> usize {
    r * cols + c
}

// Проверка, достигнут ли целевой паттерн (верхняя половина - одно значение, нижняя - другое)
fn is_target(mask: u32, rows: usize, cols: usize) -> bool {
    let half = rows / 2;
    let top_val = mask & 1;
    let bottom_val = (mask >> (rows * cols - 1)) & 1;
    if top_val == bottom_val {
        return false;
    }
    for r in 0..rows {
        let expected = if r < half { top_val } else { bottom_val };
        for c in 0..cols {
            let bit = (mask >> get_bit_index(r, c, cols)) & 1;
            if bit != expected {
                return false;
            }
        }
    }
    true
}

// Вывод доски в консоль
fn print_board(mask: u32, rows: usize, cols: usize) {
    for r in 0..rows {
        print!("| ");
        for c in 0..cols {
            let bit = (mask >> get_bit_index(r, c, cols)) & 1;
            print!("{} ", bit);
        }
        println!("|");
    }
}

fn main() {
    // Ввод режима
    println!("Выберите режим:\n1. Пример с картинки (4x3)\n2. Случайная генерация\nВаш выбор: ");
    io::stdout().flush().unwrap();
    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    let choice: u32 = input.trim().parse().unwrap_or(1);
    let user_choice = if choice == 1 { Choice::Demo } else { Choice::Gen };

    let (rows, cols) = if user_choice == Choice::Demo {
        (4, 3)
    } else {
        println!("Введите N и M (так, чтобы N * M не превышало 22 для экономии памяти): ");
        io::stdout().flush().unwrap();
        let mut dim = String::new();
        io::stdin().read_line(&mut dim).unwrap();
        let dims: Vec<usize> = dim
            .trim()
            .split_whitespace()
            .map(|x| x.parse().unwrap())
            .collect();
        (dims[0], dims[1])
    };

    let total_bits = rows * cols;
    let max_states = 1 << total_bits;

    // Начальная маска
    let start_mask = if user_choice == Choice::Demo {
        let example: [[u32; 3]; 4] = [
            [1, 0, 1],
            [0, 1, 0],
            [1, 1, 0],
            [0, 0, 1],
        ];
        let mut mask = 0;
        for r in 0..rows {
            for c in 0..cols {
                if example[r][c] == 1 {
                    mask |= 1 << get_bit_index(r, c, cols);
                }
            }
        }
        mask
    } else {
        let mut mask = 0;
        for r in 0..rows {
            for c in 0..cols {
                // Простейший генератор случайных чисел (без внешних крейтов)
                let rand_bit = (std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .unwrap()
                    .as_nanos() as u32)
                    % 2;
                if rand_bit == 1 {
                    mask |= 1 << get_bit_index(r, c, cols);
                }
            }
        }
        println!("\nСгенерирована случайная доска:");
        print_board(mask, rows, cols);
        mask
    };

    println!("\nМгновенный поиск решения через быстрый BFS...");

    // BFS
    let mut parent = vec![-1i32; max_states];
    let mut queue = VecDeque::new();
    parent[start_mask as usize] = start_mask as i32;
    queue.push_back(start_mask);

    let mut target_mask = None;

    while let Some(curr) = queue.pop_front() {
        if is_target(curr, rows, cols) {
            target_mask = Some(curr);
            break;
        }

        for r in 0..rows {
            for c in 0..cols {
                // Ход вправо
                if c + 1 < cols {
                    let idx1 = get_bit_index(r, c, cols);
                    let idx2 = get_bit_index(r, c + 1, cols);
                    let next_mask = curr ^ (1 << idx1) ^ (1 << idx2);
                    let next = next_mask as usize;
                    if parent[next] == -1 {
                        parent[next] = curr as i32;
                        queue.push_back(next_mask);
                    }
                }
                // Ход вниз
                if r + 1 < rows {
                    let idx1 = get_bit_index(r, c, cols);
                    let idx2 = get_bit_index(r + 1, c, cols);
                    let next_mask = curr ^ (1 << idx1) ^ (1 << idx2);
                    let next = next_mask as usize;
                    if parent[next] == -1 {
                        parent[next] = curr as i32;
                        queue.push_back(next_mask);
                    }
                }
            }
        }
    }

    let target_mask = match target_mask {
        Some(t) => t,
        None => {
            println!("Решение НЕ существует математически! (Конфигурация тупиковая).");
            return;
        }
    };

    // Восстановление пути
    let mut path = Vec::new();
    let mut cur = target_mask;
    while cur != start_mask {
        path.push(cur);
        cur = parent[cur as usize] as u32;
    }
    path.push(start_mask);

    // Вывод пошагового решения
    println!("\n=== НАЧАЛО ПОШАГОВОГО ВЫВОДА ===\n");
    let mut step = 0;
    for &mask in path.iter().rev() {
        println!("Шаг {}:", step);
        print_board(mask, rows, cols);
        println!();
        step += 1;
    }
    println!("Минимальное количество ходов: {}", path.len() - 1);
}