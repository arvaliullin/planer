package minio

import (
	"context"

	"github.com/minio/minio-go/v7"
	"github.com/minio/minio-go/v7/pkg/credentials"
)

// Storage реализует ports.Pinger для MinIO/S3.
type Storage struct {
	client *minio.Client
	bucket string
}

// New создаёт бакет при отсутствии и возвращает хранилище.
func New(ctx context.Context, endpoint, accessKey, secretKey string, secure bool, bucket string) (*Storage, error) {
	cl, err := minio.New(endpoint, &minio.Options{
		Creds:  credentials.NewStaticV4(accessKey, secretKey, ""),
		Secure: secure,
	})
	if err != nil {
		return nil, err
	}
	exists, err := cl.BucketExists(ctx, bucket)
	if err != nil {
		return nil, err
	}
	if !exists {
		if err := cl.MakeBucket(ctx, bucket, minio.MakeBucketOptions{}); err != nil {
			eresp := minio.ToErrorResponse(err)
			if eresp.Code != "BucketAlreadyOwnedByYou" && eresp.Code != "BucketAlreadyExists" {
				return nil, err
			}
		}
	}
	return &Storage{client: cl, bucket: bucket}, nil
}

// Ping проверяет доступ к бакету.
func (s *Storage) Ping(ctx context.Context) error {
	_, err := s.client.BucketExists(ctx, s.bucket)
	return err
}
