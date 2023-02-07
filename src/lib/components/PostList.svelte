<script lang="ts">
	import type { PostInfo } from '$lib/types/post';
	import PostPreview from './PostPreview.svelte';

	export let posts: PostInfo[];
</script>

<ul class="mt-3 mb-6 space-y-2 sm:mb-8 sm:mt-6 sm:space-y-4">
	{#each posts as post}
		<li>
			<PostPreview {post} />
		</li>
	{/each}
</ul>
