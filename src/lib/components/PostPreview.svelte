<script lang="ts">
    import type Post from '$lib/types/post';
    export let post : Post;
</script>

<section>
    <h2>{post.title}</h2>
    <p>Published {post.date}</p>
    <p>{post.excerpt}</p>
    <a href={post.slug}>Read more</a>
</section>
