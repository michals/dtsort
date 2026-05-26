-- SQLite database schema for the dtsort benchmark suite database results.

-- 1. Run Info - Global metadata for the benchmark run
CREATE TABLE IF NOT EXISTS run_info (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp TEXT,      -- ISO 8601 timestamp of the run
    cmdline TEXT,        -- Command line used to invoke the benchmark
    git_hash TEXT,       -- Git commit of the dtsort-codegen repo
    max_n INTEGER,       -- Maximum N supported by the generated dtsort
    mode TEXT,           -- Sort mode used (e.g., full, ps, ps7)
    count_iters INTEGER, -- Number of iterations for counts mode
    bench_iters INTEGER, -- Number of repetitions for wall-clock benchmarks
    sizes TEXT,          -- JSON list of large dataset sizes tested
    dt_block_size INTEGER, -- Standalone engine block size (N=128)
    dt_threshold INTEGER,  -- Hybrid threshold
    compiled_mode TEXT     -- Sort mode reported by binary (HYBRID/PS/FULL)
);

-- 2. Consistency (Small N Counts) - Raw data from .consistency.csv files
CREATE TABLE IF NOT EXISTS consistency (
    run_id INTEGER,      -- Reference to run_info(id)
    impl TEXT,           -- Implementation name (e.g., GCC14, LLVM19)
    variant TEXT,        -- 'stock' or 'dtsort'
    n INTEGER,           -- Array size (2..MAX_N)
    avg_cmp REAL,        -- Average comparison count across permutations
    max_cmp INTEGER,     -- Maximum comparison count (worst-case)
    avg_move REAL,       -- Average move count across permutations
    max_move INTEGER,    -- Maximum move count (worst-case)
    FOREIGN KEY(run_id) REFERENCES run_info(id)
);

-- 3. Large Counts - Raw data from large_counts.csv
CREATE TABLE IF NOT EXISTS large_counts (
    run_id INTEGER,      -- Reference to run_info(id)
    impl TEXT,           -- Implementation name
    variant TEXT,        -- 'stock' or 'dtsort'
    type TEXT,           -- Data type (e.g., IntNaked, WideRow)
    size INTEGER,        -- Array size (e.g., 1000, 100000)
    pattern TEXT,        -- Data distribution (random, reverse, etc.)
    avg_cmp REAL,        -- Total average comparisons
    avg_dt_cmp REAL,     -- Comparisons made inside dtsort chunks
    avg_move REAL,       -- Total average moves
    avg_dt_move REAL,    -- Moves made inside dtsort chunks
    max_cmp INTEGER,     -- Maximum comparisons in a single trial
    max_move INTEGER,    -- Maximum moves in a single trial
    FOREIGN KEY(run_id) REFERENCES run_info(id)
);

-- 4. Wall Clock (Google Benchmark) - Raw data from .json benchmark files
CREATE TABLE IF NOT EXISTS wall_clock (
    run_id INTEGER,      -- Reference to run_info(id)
    impl TEXT,           -- Implementation name
    variant TEXT,        -- 'stock' or 'dtsort'
    type TEXT,           -- Data type
    pattern TEXT,        -- Data distribution
    size INTEGER,        -- Array size
    real_time_ns REAL,   -- Wall-clock time in nanoseconds
    cpu_time_ns REAL,    -- CPU time in nanoseconds
    iterations INTEGER,  -- Number of iterations performed by Google Benchmark
    FOREIGN KEY(run_id) REFERENCES run_info(id)
);

-- 5. Weights - Raw data from weight.csv
CREATE TABLE IF NOT EXISTS type_weights (
    run_id INTEGER,      -- Reference to run_info(id)
    type TEXT,           -- Data type
    cmp_ns REAL,         -- Measured comparison cost (ns)
    move_ns REAL,        -- Measured move cost (ns)
    FOREIGN KEY(run_id) REFERENCES run_info(id)
);

-- 6. Code Size - Metadata about generated headers and object files
CREATE TABLE IF NOT EXISTS code_size (
    run_id INTEGER,      -- Reference to run_info(id)
    n INTEGER,           -- Sort size
    source_lines INTEGER,-- Number of lines in generated .hh file
    source_bytes INTEGER,-- Size of .hh file in bytes
    object_bytes INTEGER,-- Size of compiled object file in bytes
    FOREIGN KEY(run_id) REFERENCES run_info(id)
);

-- 7. Consistency Summary (Small N) - Joins stock/dtsort for side-by-side comparison
CREATE VIEW IF NOT EXISTS v_consistency_summary AS
WITH latest AS (
    SELECT *, row_number() OVER (PARTITION BY impl, variant, n ORDER BY rowid DESC) as rn
    FROM consistency
)
SELECT s.run_id, s.impl, d.variant, s.n,
       s.avg_cmp as stock_avg_cmp,   -- Baseline average comparisons
       d.avg_cmp as dtsort_avg_cmp,  -- Patched average comparisons
       s.max_cmp as stock_max_cmp,   -- Baseline worst-case comparisons
       d.max_cmp as dtsort_max_cmp,  -- Patched worst-case comparisons
       s.avg_move as stock_avg_move, -- Baseline average moves
       d.avg_move as dtsort_avg_move, -- Patched average moves
       s.max_move as stock_max_move, -- Baseline worst-case moves
       d.max_move as dtsort_max_move  -- Patched worst-case moves
FROM latest s
JOIN latest d ON s.run_id = d.run_id AND s.impl = d.impl AND s.n = d.n
WHERE s.variant = 'stock' AND d.variant != 'stock' AND s.rn = 1 AND d.rn = 1;

-- 8. Large N Summary (Total Ops and Ratios) - Joins stock/dtsort for large dataset gains
CREATE VIEW IF NOT EXISTS v_large_n_summary AS
WITH latest AS (
    SELECT *, row_number() OVER (PARTITION BY impl, variant, type, size, pattern ORDER BY rowid DESC) as rn
    FROM large_counts
)
SELECT s.run_id, s.impl, d.variant, s.type, s.size, s.pattern,
       s.avg_cmp as stock_avg_cmp,      -- Baseline total comparisons
       d.avg_cmp as dtsort_avg_cmp,     -- Patched total comparisons
       d.avg_dt_cmp as dtsort_only_cmp, -- Ops performed EXCLUSIVELY by dtsort
       s.avg_move as stock_avg_move,    -- Baseline total moves
       d.avg_move as dtsort_avg_move,   -- Patched total moves
       d.avg_dt_move as dtsort_only_move -- Ops performed EXCLUSIVELY by dtsort
FROM latest s
JOIN latest d ON s.run_id = d.run_id AND s.impl = d.impl AND s.type = d.type AND s.size = d.size AND s.pattern = d.pattern
WHERE s.variant = 'stock' AND d.variant != 'stock' AND s.rn = 1 AND d.rn = 1;

-- 9. Wall Clock Summary (Throughput) - Joins stock/dtsort for timing comparison
CREATE VIEW IF NOT EXISTS v_wall_clock_summary AS
WITH latest AS (
    SELECT *, row_number() OVER (PARTITION BY impl, variant, type, pattern, size ORDER BY rowid DESC) as rn
    FROM wall_clock
)
SELECT s.run_id, s.impl, d.variant, s.type, s.pattern, s.size,
       s.real_time_ns as stock_ns,  -- Baseline execution time (ns)
       d.real_time_ns as dtsort_ns  -- Patched execution time (ns)
FROM latest s
JOIN latest d ON s.run_id = d.run_id AND s.impl = d.impl AND s.type = d.type AND s.pattern = d.pattern AND s.size = d.size
WHERE s.variant = 'stock' AND d.variant != 'stock' AND s.rn = 1 AND d.rn = 1;

-- 10. Weight Summary (Normalized Weights) - Picks latest measured weights
CREATE VIEW IF NOT EXISTS v_weight_summary AS
WITH latest AS (
    SELECT *, row_number() OVER (PARTITION BY type ORDER BY rowid DESC) as rn
    FROM type_weights
)
SELECT run_id,
       type,     -- Data type name
       cmp_ns,   -- Measured comparison cost (ns)
       move_ns   -- Measured move cost (ns)
FROM latest
WHERE rn = 1;
