package migrations

import (
	"context"
	"database/sql"

	"github.com/pressly/goose/v3"
)

func init() {
	goose.AddMigrationContext(upInit, downInit)
}

func upInit(ctx context.Context, tx *sql.Tx) error {
	const q = `
CREATE TABLE IF NOT EXISTS planer_meta (
    id INT PRIMARY KEY DEFAULT 1 CHECK (id = 1),
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
INSERT INTO planer_meta DEFAULT VALUES ON CONFLICT DO NOTHING;
`
	_, err := tx.ExecContext(ctx, q)
	return err
}

func downInit(ctx context.Context, tx *sql.Tx) error {
	_, err := tx.ExecContext(ctx, `DROP TABLE IF EXISTS planer_meta`)
	return err
}
