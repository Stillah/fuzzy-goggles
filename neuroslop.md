# Research Proposal

**Working title:** *Deterministic Guardrails for Maintainable Agentic Code: Combining Repository-Level Graphs with Static Analysis Feedback*

---

## 1. Introduction and research problem

Coding agents have become competent at producing patches that pass a test suite. They have not become competent at leaving behind a codebase someone can keep working in. The distinction was invisible for years because the dominant evaluation protocol was a snapshot: one issue, one patch, one run of the tests. Under that protocol an agent that hard-codes a brittle special case and an agent that extends an abstraction cleanly score identically [3].

A new generation of benchmarks makes the gap measurable. SWE-CI places agents in a continuous-integration loop over 100 tasks drawn from 68 real Python repositories, where base and oracle commits are separated by an average of 233 days and 71 consecutive commits, and the agent must survive dozens of analyse-code-test rounds under an Architect–Programmer protocol [3]. The reported results are unflattering: most evaluated models introduce regressions on more than 75% of tasks, and only Claude Opus exceeded a 50% zero-regression rate [3]. SlopCodeBench attacks the same problem from the code-quality side, adding longitudinal maintainability and architecture-degradation signals beyond pass/fail, and showing that stronger models can simultaneously raise pass rates and inflate code volume several-fold [3]. SWE-Milestone reports a large performance drop when agents are asked to sustain continuous evolution rather than complete one-shot tasks, concluding that current models are optimised for single-shot completion rather than for keeping a living system alive [3].

Two failure mechanisms plausibly explain this. First, **context blindness**: the agent reasons over a retrieved slice of the repository and cannot see the structural consequences of its edit — which call sites depend on the signature it changed, which module boundary it just violated, whether the helper it wrote already exists three directories away. Similarity-based retrieval has low recall on exactly these cross-file, structural questions [2]. Second, **verification narrowness**: the only feedback signal in the loop is the test suite. A fixed test suite is a weak and gameable proxy; agents (and humans) learn to satisfy tests rather than preserve the properties that make a system maintainable [3]. EvoScore itself is acknowledged by the SWE-CI authors as a test-based proxy for maintainability rather than a direct measure of design quality [3].

Both mechanisms have known, cheap, *deterministic* countermeasures that the agent literature has mostly used for one-shot correctness rather than for long-horizon maintainability. Repository-level graphs give structure-aware context: the Repository Planning Graph (RPG) unifies a functional view (features, requirements) with a structural view (files, classes, functions, dependencies) and has been used as a control layer for long-horizon coding agents [1]; CodexGraph indexes repositories by static analysis into a graph database the agent queries directly, raising retrieval precision over similarity search [2]. Static analysis feedback loops give property-level verification: static analysis as a feedback loop has been shown to substantially improve maintainability of LLM-generated code [4], and CORE demonstrates a propose-rank loop driven by CodeQL and SonarQube findings across Python and Java [4].

**The research problem.** These two ingredients have never been combined into a single persistent control layer and evaluated on benchmarks that actually measure long-horizon maintainability. The graph work targets *generation from scratch* [1] or *retrieval for one-shot tasks* [2]. The static analysis work targets *one-shot revision quality* [4]. Nobody has asked whether a deterministic, non-LLM guardrail layer — a live repository graph plus a battery of analysers, linters, type checkers, deprecation and architecture-constraint checkers — can keep an agent's code maintainable across dozens of CI iterations, and whether the resulting maintainability gain costs or buys functional progress.

---

## 2. Research question

**Main research question**

> To what extent can a deterministic guardrail layer — combining a continuously maintained repository-level graph with static analysers, linters, type checkers, and deprecation/architecture-constraint checkers — improve the long-horizon maintainability of code produced by LLM coding agents, without sacrificing functional progress?

**Sub-questions**

- **RQ1 (Characterisation).** What kinds of maintainability degradation do current agents actually produce over long-horizon CI loops, how do these failures distribute across iterations, and which of them are detectable by deterministic tooling at the moment of introduction?
- **RQ2 (Graph contribution).** Does structure-aware context from a live repository graph — impact sets of a proposed edit, duplicate-functionality candidates, dependency and layering violations, API-surface diffs — reduce regressions and structural degradation compared with a retrieval-only agent?
- **RQ3 (Checker contribution).** Does a deterministic checker gate placed between the agent's edit and the commit reduce maintainability degradation, and does *gating* (reject and require revision) differ measurably from *advising* (surface findings in context)?
- **RQ4 (Composition and cost).** Do the graph and checker layers compose, or do their effects overlap? What is the token, wall-clock, and iteration-budget overhead, and does the improvement survive a budget-matched comparison against the unaugmented agent?
- **RQ5 (Measurement validity).** How well do benchmark-native scores (EvoScore, zero-regression rate, normalized change [3]; SlopCodeBench degradation signals [3]) correlate with direct static maintainability measures, and where do they disagree?

---

## 3. Novelty and contribution

**What is new.** The thesis inverts the direction of existing graph work. RPG and ZeroRepo use a graph as a *forward* blueprint to generate a repository in dependency-aware order [1]; RPG-Encoder generalises RPG from a static generative blueprint into a representation of an existing project [1]. CodexGraph uses a graph as a *read* interface for context retrieval [2]. This work uses a graph as a **continuously updated maintainability contract**: the graph is rebuilt incrementally after every agent iteration, and the *delta* between consecutive graphs becomes a first-class verification signal — new import cycles, widened fan-in, duplicated subgraphs, broken layering, removed public symbols. To my knowledge no prior work treats graph deltas as an evolution-time maintainability oracle in an agent loop.

Second, existing static-analysis-in-the-loop work evaluates one-shot revision quality on quality-issue benchmarks [4]. This work evaluates a deterministic feedback layer on benchmarks explicitly designed around long-horizon degradation [3], which allows a question nobody has answered: does deterministic feedback *compound* over iterations, or does its benefit wash out as the agent accumulates architectural debt?

**Planned contributions**

1. **C1 — An empirical taxonomy of agentic maintainability degradation** over long-horizon CI loops, derived from recorded agent trajectories on SWE-CI, SlopCodeBench and SWE-Milestone, with per-category detectability by off-the-shelf deterministic tooling. This is a standalone contribution independent of whether the intervention works.
2. **C2 — A reusable maintainability instrumentation harness** that snapshots a repository graph plus a fixed analyser battery after every agent iteration, producing per-iteration trajectories of structural and quality metrics. Released open source, harness-agnostic, usable by others on any of the three benchmarks.
3. **C3 — The guardrail layer itself**: a graph service with an impact-analysis and duplicate-detection query API, plus a configurable deterministic gate, both exposed to the agent as tools and as a pre-commit policy.
4. **C4 — A controlled ablation** separating graph-context, checker-advice, and checker-gating effects, under both free and budget-matched conditions, across three benchmarks and at least two model backends.
5. **C5 — A validity analysis** of maintainability proxies: correlation between benchmark-native scores and direct static measures, plus a documented account of where agents satisfy the checkers while degrading the design (checker-level Goodharting).

---

## 4. State of the art

**Repository-level graph representations.** RPG is a hierarchical dual-view graph in which nodes hold files, classes and functions while edges capture semantic relations, data flows and implementation order, unifying proposal-level and implementation-level planning [1]. ZeroRepo operates in three stages — proposal-level planning, implementation-level refinement, then graph-guided generation with test validation — traversing the graph in topological order with test-driven development, guided localisation and iterative editing [1]. On the RepoCraft benchmark (6 projects, 1,052 tasks) it reports large margins over agent baselines, including 27.3 points on functionality coverage and 35.8 points on test pass rate against Claude Code [1]. RPG-Encoder extends the representation to arbitrary existing projects via tree-sitter entity extraction and import/invocation/inheritance edges [1]. The limitation for this thesis: RPG is a generation-time blueprint, evaluated on from-scratch construction, not on sustained maintenance of a repository the agent did not design.

**Graph-based context retrieval.** CodexGraph indexes repositories with static analysis into a graph database under a unified schema, with nodes for modules, classes and functions and edges for inheritance and usage, and lets an LLM agent compose and execute graph queries for precise, structure-aware retrieval [2]. It uses shallow indexing followed by deeper cross-file resolution, and a write-then-translate strategy for query generation [2]. It is evaluated on CrossCodeEval, SWE-bench and EvoCodeBench — all snapshot-style — so its effect on maintainability over time is unmeasured. Code property graphs are the older, deeper lineage here, built for queryable structural and dataflow analysis of source code [2], and codebase knowledge graph practice generally follows parse-to-AST, extract entities, build edges, store in a graph query layer [2].

**Deterministic tooling in LLM loops.** Static analysis as a feedback loop measurably improves maintainability of generated code, with Bandit and Pylint used across readability, maintainability, functionality and security dimensions [4]. CORE pairs a proposer LLM that applies tool recommendations with a ranker LLM that filters unintended functional changes, working across Python and Java and across CodeQL and SonarQube, with fix rates comparable to rule-based program repair at far lower engineering cost [4]. Combining correctness testing with static vulnerability analysis yields 59% success on correctness issues and 89% on safety vulnerabilities in a C study using Infer [4]. Agent surveys note that static analysis is already used to supply valid symbols and inter-code dependencies during generation, and that injecting analysed information into the prompt guides models toward valid code [2]. Sonar's synthesis of the literature concludes that LLM-generated code carries quality and security risks requiring independent static analysis to detect reliably [4]. Common to all of this: single-shot or short-loop settings, quality measured at one point in time.

**Long-horizon maintainability benchmarks.** SWE-CI is the first repository-level benchmark built on the CI loop, with 100 tasks over 68 Python repositories, Architect–Programmer dual-agent evaluation, up to 20 iterations, and metrics EvoScore (iteration-weighted progress, later rounds weighted higher), normalized change, and zero-regression rate [3]. SlopCodeBench benchmarks how coding agents degrade over long-horizon iterative tasks via checkpoint-based evaluation, adding architecture-degradation signals [3]. SWE-Milestone evaluates agents on continuous software evolution and reports large drops relative to one-shot settings [3]. Related efforts include SWE-EVO on long-horizon evolution and RoadmapBench on version upgrades [3]. Known limitations I must design around: Python-only, fixed oracle test suites that create a measurable optimisation target, and dependence on a specific dual-agent harness [3].

**The gap.** Graph representations are used for generation and retrieval but not as evolution-time maintainability contracts. Deterministic analysers are used for one-shot quality repair but not as a sustained guardrail across CI iterations. Long-horizon maintainability benchmarks exist but, as far as published work goes, have been used to *characterise* model weakness rather than to evaluate *interventions* against it. This thesis occupies that intersection.

---

## 5. Goal of the thesis

**Primary goal.** Design, implement and empirically evaluate a deterministic guardrail layer that improves the long-horizon maintainability of agent-generated code, and quantify the trade-off against functional progress and cost on SWE-CI, SlopCodeBench and SWE-Milestone.

**System design (target architecture).**

- **Graph layer.** Incremental repository graph over tree-sitter plus a Python-aware indexer (scip-python / Jedi / Griffe), dual-view in the RPG sense — structural nodes and edges, plus a feature/requirement view linked to code nodes [1]. Stored in an embedded graph store (Kùzu or Neo4j) and exposed to the agent as query tools in the CodexGraph style [2], plus a non-negotiable **graph-diff verifier** run after each iteration.
- **Checker layer.** A fixed, versioned battery organised by the maintainability property it defends:
  - *Correctness-adjacent:* Ruff, Pylint, mypy or pyright, Bandit, Semgrep (with custom architectural rules).
  - *Structure and architecture:* import-linter / Grimp for layering contracts, Tach, deptry for unused and missing dependencies, graph-derived cycle, fan-in/fan-out and modularity checks.
  - *Bloat and duplication:* Vulture or deadcode for unreachable code, jscpd or PMD-CPD for clone detection, Radon and Lizard for cyclomatic complexity, Halstead volume and Maintainability Index, Wily for metric trends across the iteration history.
  - *Deprecation and API hygiene:* Griffe API diffs to detect silent public-surface breakage, pyupgrade and flake8-deprecated for outdated idioms, pytest with `-W error::DeprecationWarning` to turn runtime deprecation signals into hard failures, pip-audit for dependency advisories.
  - *Test-suite strength:* coverage.py with diff-cover on the patch, and mutation testing (mutmut) on changed modules as a non-gameable complement to a fixed oracle suite.
- **Policy layer.** Three configurable modes per checker group — *off*, *advise* (findings injected into agent context), *gate* (edit rejected, structured finding returned, revision required) — which is what makes RQ3 answerable.

**Hypotheses.**

- **H1.** Graph-derived impact analysis reduces zero-regression failures relative to a retrieval-only baseline, because most regressions originate in unobserved call sites.
- **H2.** Gating on structure and bloat checks reduces code-volume inflation and complexity growth at equal or better functional progress.
- **H3.** Graph and checker effects partially overlap; the combination outperforms either alone but by less than the sum of their individual effects.
- **H4.** A non-trivial share of improvement under free budget is attributable to extra iterations, and shrinks under budget matching — which, if true, is a finding worth publishing on its own.

**Scope and non-goals.** Python only, matching benchmark coverage [3]. No model fine-tuning. No LLM-as-judge as a primary metric; LLM judging may appear only as a secondary, clearly-labelled signal. The guardrail layer must stay deterministic, which is the whole point: it is reproducible, auditable and cheap relative to additional model calls.

**Success criteria.**

- *Minimum:* C1 and C2 delivered; at least one intervention arm fully evaluated on SWE-CI with statistically defensible comparison against baseline.
- *Target:* all ablation arms on SWE-CI and SlopCodeBench, with a measured improvement in zero-regression rate and reduced code-bloat growth at non-inferior EvoScore.
- *Stretch:* SWE-Milestone included, two model backends, and a workshop or conference submission.

---

## 6. Detailed work plan and schedule

**Budget.** ~10 h/week, mid-October through late May, ≈ 32 calendar weeks minus ~2 weeks holiday slack ≈ **300 hours**. Writing is continuous from WP2 onward rather than deferred to the end; the thesis document is started in week 5 and grows with each work package.

| WP | Period | Hours | Content | Deliverable / milestone |
|---|---|---|---|---|
| **WP0** Scoping and literature | mid-Oct – early Nov (wk 1–3) | 30 | Systematic review: graph-based repo representation [1][2], static analysis in LLM loops [4], long-horizon benchmarks [3]. Verify benchmark availability, licences, harness code, API cost model. Fix the research questions with supervisor. | **M1 (early Nov):** proposal frozen, annotated bibliography, cost estimate |
| **WP1** Baseline harness | early Nov – early Dec (wk 4–7) | 40 | Reproduce SWE-CI's Architect–Programmer loop on a 15–20 task pilot subset [3]. Containerised, seeded, resumable runs. Confirm reported baseline numbers are within reproduction tolerance. Set up SlopCodeBench next. | **M2 (early Dec):** reproducible baseline, pilot numbers vs. published |
| **WP2** Measurement infrastructure | early Dec – mid-Jan (wk 8–12, incl. holiday slack) | 45 | Build the instrumentation harness (C2): per-iteration snapshot of graph + analyser battery → metric trajectories. Define the maintainability metric set and its aggregation. First-draft thesis chapters 1–3 (intro, related work, background). | **M3 (mid-Jan):** instrumentation harness released; chapters 1–3 drafted |
| **WP3** Degradation study | mid-Jan – early Feb (wk 13–15) | 30 | Run instrumentation over baseline trajectories. Build the failure taxonomy (C1): manual coding of a sampled set of degrading edits, cross-referenced against which checkers fired. Produces the prioritised tool list for WP4–WP5. | **M4 (early Feb):** taxonomy + detectability table; this is the empirical core of the thesis if the intervention underdelivers |
| **WP4** Graph layer | early Feb – early Mar (wk 16–19) | 40 | Incremental dual-view graph [1], agent-facing query tools [2], graph-diff verifier. Benchmark indexing latency — it must be cheap enough to run every iteration. Pilot-subset evaluation of the graph-only arm. | **M5 (early Mar):** graph layer working, graph-only pilot results |
| **WP5** Checker layer and policy | early Mar – early Apr (wk 20–23) | 40 | Checker battery wired behind a unified finding schema; advise vs. gate modes; deprecation and API-diff checks; architectural contracts per repository. Noise control: a gate that fires constantly is a gate that gets bypassed. Pilot-subset checker-only arm. | **M6 (early Apr):** full guardrail layer, checker-only pilot results |
| **WP6** Main evaluation | early Apr – mid-May (wk 24–29) | 50 | Full runs: baseline / graph-only / checkers-advise / checkers-gate / combined, plus budget-matched combined. SWE-CI first (full 100 tasks), then SlopCodeBench, then SWE-Milestone if budget allows. Multiple seeds for nondeterminism. Statistical analysis, correlation study for RQ5. | **M7 (mid-May):** complete results tables, ablation analysis |
| **WP7** Write-up and defence prep | mid-May – late May (wk 30–32) | 25 | Finish method, results, discussion, threats-to-validity and future-work chapters. Artefact packaging and reproducibility appendix. Defence slides. | **M8 (late May):** thesis submitted |

**Checkpoint logic.** Each milestone is a go/no-go. If M2 slips past mid-December, SWE-Milestone is dropped from scope. If M5 slips, the graph layer is reduced to a read-only impact-analysis tool without the generative feature view. If compute or API budget binds in WP6, the full-benchmark run is restricted to SWE-CI plus a stratified SlopCodeBench subset, with the reduction documented rather than hidden.

**Risks and mitigations.**

| Risk | Mitigation |
|---|---|
| Benchmark harnesses not reproducible or not released | Pilot reproduction is WP1, deliberately early. Fallback: construct an equivalent CI-loop harness over the published task definitions and report it as a reimplementation. |
| API cost exceeds budget | Pilot subsets throughout; open-weight backend as the primary model with a frontier model used only for a confirmatory arm; cache and resume aggressively. |
| Guardrail layer produces no measurable improvement | The taxonomy (C1), instrumentation harness (C2) and validity analysis (C5) stand alone. A rigorous negative result on a plausible intervention is a legitimate thesis outcome, and the taxonomy explains *why*. |
| Checker noise degrades agent performance | Advise/gate separation is designed in from the start; per-group tuning on the pilot subset; measure finding-acceptance rate as a diagnostic. |
| Fixed oracle suites are gameable [3] | Mutation testing and graph-diff metrics are independent of the oracle suite; report them alongside benchmark-native scores. |
| Nondeterminism of agent runs | Fixed seeds and temperature where the API allows; repeated runs on a subset to estimate variance; report confidence intervals, not point estimates. |
| Benchmark data contamination | Report contamination risk explicitly; prefer repositories with base commits post-dating model cutoffs where the benchmark allows the distinction. |

---

## 7. References

Sources marked † were located through the context search for this proposal; bibliographic details should be re-verified against the publisher record before final submission, particularly for the three benchmark papers.

**Repository graphs and graph-based agent context**

1. † Microsoft Research. *RPG: A Repository Planning Graph for Unified and Scalable Codebase Generation.* ICLR 2026. arXiv:2509.16198. https://arxiv.org/abs/2509.16198 — see also the reference implementation, `microsoft/RPG-ZeroRepo`, https://github.com/microsoft/RPG-ZeroRepo [1]
2. † *Universal Repository Representation with RPG-Encoder.* ICML 2026 poster 63029. https://icml.cc/virtual/2026/poster/63029 [1]
3. † Liu, X. et al. *CodexGraph: Bridging Large Language Models and Code Repositories via Code Graph Databases.* NAACL 2025 (long). arXiv:2408.03910. https://aclanthology.org/2025.naacl-long.7.pdf [2]
4. Yamaguchi, F., Golde, N., Arp, D., Rieck, K. *Modeling and Discovering Vulnerabilities with Code Property Graphs.* IEEE Symposium on Security and Privacy, 2014.
5. † *Codebase Knowledge Graphs: Architecture, Applications, Challenges and Trends.* Technical review, atoms.dev, 2026. [2]

**Long-horizon maintainability benchmarks**

6. † Chen, J. et al. *SWE-CI: Evaluating Agent Capabilities in Maintaining Codebases via Continuous Integration.* arXiv:2603.03823, March 2026. https://arxiv.org/abs/2603.03823 [3]
7. † *SlopCodeBench: Benchmarking How Coding Agents Degrade Over Long-Horizon Iterative Tasks.* 2026. ResearchGate publication 403194539. [3]
8. † *SWE-Milestone: Evaluating AI Agents on Continuous Software Evolution.* arXiv:2603.13428, 2026. [3]
9. Jimenez, C. E., Yang, J., Wettig, A., Yao, S., Pei, K., Press, O., Narasimhan, K. *SWE-bench: Can Language Models Resolve Real-World GitHub Issues?* ICLR 2024. arXiv:2310.06770.
10. Yang, J., Jimenez, C. E., Wettig, A., Lieret, K., Yao, S., Narasimhan, K., Press, O. *SWE-agent: Agent-Computer Interfaces Enable Automated Software Engineering.* NeurIPS 2024. arXiv:2405.15793.
11. † *SWE-EVO: Benchmarking Coding Agents in Long-Horizon Software Evolution Scenarios.* 2025. [3]
12. † *Multi-SWE-bench: A Multilingual Benchmark for Issue Resolving.* NeurIPS 2025. [3]

**Static analysis, linters and deterministic feedback for generated code**

13. † *Static Analysis as a Feedback Loop: Enhancing LLM-Generated Code Beyond Correctness.* 2025. ResearchGate publication 396421618. [4]
14. † Microsoft Research. *Frustrated with Code Quality Issues? LLMs can help* (CORE: proposer–ranker loop over CodeQL and SonarQube findings). [4]
15. † *Helping LLMs Improve Code Generation Using Feedback from Testing and Static Analysis.* Discover Artificial Intelligence, 2026. doi:10.1007/s44163-026-01009-5. [4]
16. † *ReCatcher: Towards LLMs Regression Testing for Code Generation.* arXiv:2507.19390. [4]
17. † Nunez, A. et al. *AutoSafeCoder: A Multi-Agent Framework for Securing LLM Code Generation through Static Analysis and Fuzz Testing.* arXiv, 2024. [2]
18. † SonarSource. *LLMs for Code Generation: A Summary of the Research on Quality.* 2026. [4]
19. † *LLM-Driven Code Refactoring: Research Directions.* Software Analysis and Intelligence Lab, Queen's University, 2025. [4]

**Agent architectures and surveys**

20. Yao, S., Zhao, J., Yu, D., Du, N., Shafran, I., Narasimhan, K., Cao, Y. *ReAct: Synergizing Reasoning and Acting in Language Models.* ICLR 2023. arXiv:2210.03629.
21. † Wang, X. et al. *Executable Code Actions Elicit Better LLM Agents* (CodeAct). ICML 2024. [2]
22. † *Large Language Model-Based Agents for Software Engineering: A Survey.* arXiv:2409.02977, 2024. [2]
23. † Wang, X. et al. *OpenHands: An Open Platform for AI Software Developers as Generalist Agents.* arXiv, 2024. [2]

**Software maintainability foundations**

24. McCabe, T. J. *A Complexity Measure.* IEEE Transactions on Software Engineering, SE-2(4), 1976.
25. Oman, P., Hagemeister, J. *Metrics for Assessing a Software System's Maintainability.* ICSM 1992.
26. Lehman, M. M. *Programs, Life Cycles, and LAnthropic of Software Evolution.* Proceedings of the IEEE, 68(9), 1980.
27. Kruchten, P., Nord, R. L., Ozkaya, I. *Technical Debt: From Metaphor to Theory and Practice.* IEEE Software, 29(6), 2012.

