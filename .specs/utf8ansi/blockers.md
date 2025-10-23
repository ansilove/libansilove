# Task Dependency Graph

```
task-demo (FINAL)
├─ BLOCKED BY: task-color-fix
│  └─ BLOCKED BY: task-tests
│     └─ UNBLOCKED
├─ BLOCKED BY: task-build-clean
│  └─ UNBLOCKED
└─ BLOCKED BY: task-state-machine-verify
   └─ UNBLOCKED

UNBLOCKED TASKS (can start immediately):
- task-tests
- task-build-clean  
- task-state-machine-verify

EXECUTION PLAN:
1. Verify state machine fix (quick check)
2. Build test suite (task-tests)
3. Run tests to confirm color bug
4. Apply color fix (remove line 333)
5. Verify tests pass
6. Clean build system
7. Run final demo
```

## Critical Path
task-tests → task-color-fix → task-demo

## Estimated Effort
- task-state-machine-verify: 5min
- task-tests: 20min
- task-color-fix: 10min
- task-build-clean: 15min
- task-demo: 10min

**Total: ~60min**
