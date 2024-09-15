#pragma once

// SBIの処理結果を返す構造体
struct sbiret {
    long error;
    long value;
};